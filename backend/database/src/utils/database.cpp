#include "database.h"
#include <iostream>
#include <fstream>

/**
 * @brief Opens the header file, jumps to offset, and reads data into itself.
 * @param headerFilePath - Path where the Header File is located.
 * @param byteOffset - Byte offset of where header is located.
 * @return true if header had data from file read into it
 * and file bounds are >= sizeof(Header), false otherwise.
 */
bool Header::getData(const std::string& headerFilePath, uint64_t byteOffset)
{
	// Open a Binary Read-Only file stream of the Header File.
	std::fstream headerFile(headerFilePath, std::ios::in | std::ios::binary);
	if (!headerFile.is_open()) // Handle failing to open file.
	{
		std::cerr << "[Header] Error: Could not open header file." << std::endl;
		// Close file stream.
		headerFile.close();
		return false;
	}
	// Move to end of file.
	headerFile.seekg(0, std::ios::end);
	// Get file size based on file pointer position.
	uint64_t fileSize = headerFile.tellg();
	
	// Get the size of the Header Object in bytes.
	const uint64_t HEADER_SIZE_BYTES = sizeof(*this);
	
	// If the size of desired data at the Byte Offset exceeds file size, file is corrupted.
	if (byteOffset + HEADER_SIZE_BYTES > fileSize)
	{
		// Close file stream.
		headerFile.close();
		return false;
	}
	
	// Move file pointer to the Byte Offset.
	headerFile.seekg(byteOffset, std::ios::beg);
	// Read data into the Header Object, based on it's size in bytes.
	headerFile.read(reinterpret_cast<char*>(this), HEADER_SIZE_BYTES);
	
	// Close file stream.
	headerFile.close();
	// Data read successfully.
	return true;
}

/**
 * @brief Checks if the header was able to successfully populate data into it.
 * @details By default, data is corrupted because nothing has been populated.
 * If the method getData was run and no data was read, the data is considered
 * corrupted.
 * @return true if all attributes != UINT64_MAX, false otherwise.
 */
bool Header::isCorrupted()
{
	// If the default values of the start/end position attributes are present, data is corrupted.
	return (this->startPosition == UINT64_MAX && this->endPosition == UINT64_MAX);
}

/**
 * @brief Checks if previous header exists.
 * @param header Takes a header formatter as a pointer.
 * @return Header* with formatted data if read successfully,
 * Header* with default values if truncation occured,
 * Header* with default values if read was unsuccessful,
 * nullptr if file was empty.
 */
Header* BTree::getPrevHeader(Header *header)
{
	// Get the size of the Header Object in bytes.
	const uint64_t HEADER_SIZE_BYTES = sizeof(*header);
	if (this->headerFileSize_ == 0) // Handle no headers existing.
	{
		return nullptr; 
	}
	// Handle insufficient bytes for header, while Header File isn't empty.
	if (this->headerFileSize_ < HEADER_SIZE_BYTES) 
	{
		std::cerr << "[BTree] Error: Header file truncation occurred." << std::endl;	
		// Return a corrupted Header with default attribute values.
		return header;
	}
	// Handle errors from Header getting data.
	if (!header->getData(this->headerFilePath_, this->headerFileSize_ - HEADER_SIZE_BYTES))
	{
		std::cerr << "[BTree] Error: Failed to read previous header." << std::endl;
		// Return a corrupted Header with default attribute values.
		return header;
	}

	// If the previous if statement succeeds, then Header is populated with proper attribute values 
	return header;
}

/**
 * @brief Creates Data File and Header File and gets file size.
 * @param headerFilePath File path for the Header File.
 * @param dataFilePath File path for Data File.
 */
BTree::BTree(const std::string& headerFilePath, const std::string& dataFilePath) :
	headerFilePath_(headerFilePath), 
	dataFilePath_(dataFilePath), 
	headerFileSize_(0),
	dataFileSize_(0)
{
	// Open a Binary Append file stream of the Header File.
	std::fstream headerFile(this->headerFilePath_, std::ios::app | std::ios::binary);
	
	// Open a Binary Append file stream of the Data File.
	std::fstream dataFile(this->dataFilePath_, std::ios::app | std::ios::binary);

	if (headerFile.is_open())
	{
		// Set file pointer to end of file.
		headerFile.seekp(0, std::ios::end);
		// Set attribute file size based on pointer position.
		this->headerFileSize_ = headerFile.tellp();
		// Close file stream.
		headerFile.close();
	}

	if (dataFile.is_open())
	{
		// Set file pointer to end of file.
		dataFile.seekp(0, std::ios::end);
		// Set attribute file size based on pointer position.
		this->dataFileSize_ = dataFile.tellp();
		// Close file stream.
		dataFile.close();
	}
}

/**
 * @brief Creates/appends a header for Header File, writes
 * inputted data to Data File.
 * @param data Takes inputted data to be written in the Data File.
 * @return uint64_t Byte Offset of where the header was written in
 * Header File.
 */
uint64_t BTree::writeData(const std::string& data)
{
	const size_t maxDataSize = 1073741824; // One gigabyte in bytes

	// If adding new data will exceed file size, then throw an error.
	if (data.size() + this->dataFileSize_ > maxDataSize)
	{
		std::cerr << "[BTree] Error: Cannot add data (Data would exceed limits)." << std::endl;
		// Throw error value.
		return UINT64_MAX;
	}
	
	// Open a Binary Append file stream of the Header File.
	std::fstream headerFile(this->headerFilePath_, std::ios::app | std::ios::binary);
	if (!headerFile.is_open())
	{
		std::cerr << "[BTree] Error: Could not open header file." << std::endl;
		// Throw error value.
		return UINT64_MAX;
	}

	// Open a Binary Append file stream of the Data File.
	std::fstream dataFile(this->dataFilePath_, std::ios::app | std::ios::binary);
	if (!dataFile.is_open())
	{
		std::cerr << "[BTree] Error: Could not open data file." << std::endl;
		// Throw error value.
		return UINT64_MAX;
	}
	
	// Initialize a Header Object to receive formatted data.
	Header prevHeader; 
	// Get output of previous header method by passing in prevHeader by reference.
	Header* result = this->getPrevHeader(&prevHeader);

	// Handle if file wasn't empty but previous header was corrupted/not 
	// formatted correctly.
	if (result != nullptr && prevHeader.isCorrupted())
	{
		// Close file stream.
		headerFile.close();
		// Close file stream.
		dataFile.close();
		// Throw error value.
		return UINT64_MAX;
	}
	
	// Initialize a Header Object to write formatted data.
	Header newHeader;
	// If the Header File is empty.
	if (result == nullptr)
	{
		// If no header has been written, then no data is in
		// the data file. Start data writing/storage at the start
		// of the data file.
		newHeader.startPosition = 0;
	}
	else
	{
		// If a previous header exists, start data writing/storage
		// at the end of previous data.
		newHeader.startPosition = prevHeader.endPosition;
	}
	// Make the end data writing/storage position based on the offset
	// from the start of the file + the size of the data.
	newHeader.endPosition = data.size() + newHeader.startPosition; 
	
	// Write the Header into the Header File.
	headerFile.write(reinterpret_cast<char*>(&newHeader), sizeof(Header));

	// Write the data into the Data File.
	dataFile.write(data.data(),data.size());

	// Get Header Byte Offset based on the file size before adding a
	// new Header.
	uint64_t headerOffset = this->headerFileSize_;
	
	// Update Header File size to account for new Header.
	this->headerFileSize_ += sizeof(Header);
	// Update Data File size with the size of the new data added.
	this->dataFileSize_ += data.size();

	// Close all file streams.
	headerFile.close();
	dataFile.close();

	// Return the Header Byte Offset.
	return headerOffset;
}

/**
 * @brief Searches/queries and returns data based on header offset.
 * @param offset Byte Offset of header in Header File.
 * @return std::string Data stored within the Data File in specified
 * location.
 */
std::string BTree::queryData(uint64_t offset)
{
	// Open a Binary Read-Only file stream for the Data File.
	std::fstream dataFile(this->dataFilePath_, std::ios::in | std::ios::binary);
	if (!dataFile.is_open())
	{
		std::cerr << "[BTree] Error: Could not open data file." << std::endl;
		// Return empty string as a form of error.
		return "";
	}
	// Create Header Object to retrieve formatted data.
	Header header;

	// If the Header at the inputted Byte Offset couldn't be read, throw error.
	if (!header.getData(this->headerFilePath_, offset))
	{
		std::cerr << "[BTree] Error: Header could not be retrieved during query.";
		// Close file stream.
		dataFile.close();
		// Return empty string as a form of error.
		return "";
	}
	
	// Calculate the size of the data based on Header start and end position.
	uint64_t dataSize = header.endPosition - header.startPosition;
	
	// If the data has a size of zero, throw error.
	if (dataSize == 0)
	{
		// Close file stream.
		dataFile.close();
		// Return empty string as a form of error.
		return "";
	}
	
	// Set Data File pointer to the start of data.
	dataFile.seekg(header.startPosition, std::ios::beg);

	// Create a string and set it's size to data's size.
	std::string result;
	result.resize(dataSize);

	// Read raw data into the string.
	dataFile.read(result.data(), dataSize);
	// Close file stream.
	dataFile.close();
	return result;
}

/**
 * @brief Creates/opens Map File and attempts to initialize                                         
 * dataPointers_ with previous data.
 */
DataBase::DataBase() : dataStorage_("../database/data/header.bin", "../database/data/dataPool.bin"), 
	mapFilePath_("../database/data/map.bin")
{
	// Open a Binary Read-Only file stream of Map File.
	std::fstream mapFile(this->mapFilePath_, std::ios::in | std::ios::binary);
	if (mapFile.is_open())
	{
		// Set map file pointer to the end of file.
		mapFile.seekg(0, std::ios::end);
		// Get filse size from current pointer offset.
		size_t fileSize = mapFile.tellg();
		// Set map file pointer back to the beginning of the file.
		mapFile.seekg(0, std::ios::beg);

		// Only read if the file has complete uint64_t Key-Value pairs.
		if (fileSize > 0 && (fileSize % (sizeof(uint64_t) * 2) == 0))
		{
			// While end of file hasn't been reached.
			while (mapFile.peek() != EOF)
			{
				// Create a Key variable for retrieval.
				uint64_t hashKey = 0;
				// Create a Value variable for retrieval.
				uint64_t offset = 0;
				
				// Write raw data into Key.
				mapFile.read(reinterpret_cast<char*>(&hashKey), sizeof(hashKey));
				// Write raw data into Value.
				mapFile.read(reinterpret_cast<char*>(&offset), sizeof(offset));

				if (mapFile) // Ensure read succeeded without hitting unexpected EOF
				{
					// Add the Key-Value pair into the dictionary.
					this->dataPointers_[hashKey] = offset;
				}
			}
		}
		// Close file stream.
		mapFile.close();
	}
}

/**
 * @brief Generate a hash for a given Key.
 * @details Will not perform hashing for Keys which size is less than or equal
 * to 8 chars, but will only convert it to a uint64_t.
 * @param inputted Takes an inputted string and converts it to a uint64_t hash.
 * @return uint64_t Final hash value of the inputted string.
 */
uint64_t DataBase::stringToHash(const std::string& input)
{
	uint64_t finalHash = 0;
	
	// Process input in chunks of 8.
	for (size_t i = 0; i < input.size(); i += 8)
	{
		uint64_t smallHash = 0;
		// Check the number of bytes left. If its >= 8 then it defaults to 8
		// for processing.
		size_t bytesLeft = (8 > input.size() - i) ? (input.size() - i) : 8;
		// Process the input one byte at a time, based on how much is left.
		for (int ii = 0; ii < bytesLeft; ++ii)
		{
			// Get the current char, make it unsigned, and convert it into a uint64_t.
			uint64_t byte = static_cast<uint64_t>(static_cast<unsigned char>(input[i + ii]));
			// Pack bytes together by shifting down by 8 bits.
			smallHash |= (byte << (ii * 8));
		}
		// Scramble the resulting hash. Leave it alone if < or = to 8 chars.
		finalHash *= i + 1;
		// XOR/toggle the bits of the resulting hash with the packed bytes.
		finalHash ^= smallHash;
		
		// Apply a rotation to resulting hash to scramble.
		size_t rotation = i % 64;
		
		// Calculate rotation.
		rotation > 0 ? finalHash = (finalHash << rotation) | (finalHash >> (64 - rotation)) : 0;

	}
	return finalHash;
}

/**
 * @brief Writes a Key-Value pair into the BTree.
 * @details The inputted Key string will get converted into a uint64_t hash
 * via stringToHash. The value that will be used within dataPointers_ will be
 * the Header File Byte Offset for the data which is gotten via writeData in
 * the BTree.
 * @param key Used for finding data in dataPointers_.
 * @param data Will be written to BTree.
 * @return true if Writing succeeds, false otherwise.
 */
bool DataBase::writeData(const std::string& key, const std::string& data)
{
	// Create a multi-threading lock that only allows for one process at a time.
	std::lock_guard<std::mutex> lock(this->mutex_);
	
	// Open a Binary Append file stream on the Map File.
	std::fstream mapFile(this->mapFilePath_, std::ios::app | std::ios::binary);
	if (!mapFile.is_open())
	{
		std::cerr << "[DataBase] Error: Could not open map file." << std::endl;
		// Close file stream.
		mapFile.close();
		return false;
	}
	// Write data on BTree and get offset of the Header.
	uint64_t offset = this->dataStorage_.writeData(data);
	// If writing to the BTree had an error.
	if (offset == UINT64_MAX)
	{
		std::cerr << "[DataBase] Error: Could not write data to BTree." << std::endl;
		// Close file stream.
		mapFile.close();
		return false;
	}
	// Generate a hash for the key.
	uint64_t keyHash = stringToHash(key);

	// Write the Key to the Map File.
	mapFile.write(reinterpret_cast<char*>(&keyHash), sizeof(keyHash));
	// Write the Byte Offset of the Header to the Map File.
	mapFile.write(reinterpret_cast<char*>(&offset), sizeof(offset));

	// Close file stream.
	mapFile.close();
	// Add Key-Value pair to dictionary.
	this->dataPointers_[keyHash] = offset;

	
     // Print out console log, if successful.
     std::cout << "[DataBase] Write Query Processed: " << key
         << " | Hash Key: " << keyHash
         << " | Header Offset: " << offset
         << " | Input: " << data << std::endl;

	return true;
}

/**
 * @brief Takes a Key, checks if there is a header, and attempts to return data.
 * @details Key is converted into a uin64_t hash via stringToHash. Searches for
 * Key-Value pair in dataPointers_ where value is the Byte Offset to the header.
 * Attempts to read header from Header File and retrieve data from Data File.
 * @param key Used for finding data in dataPointers_.
 * @return std::string Data if data retrieved successfully retrieved from BTree.
 * empty string otherwise.
 */
std::string DataBase::queryData(const std::string& key)
{
	// Create a multi-threading lock that only allows for one process at a time.
	std::lock_guard<std::mutex> lock(this->mutex_);
	
	// Converts inputted key into a hash.
	uint64_t hashKey = this->stringToHash(key);
	// Searches in dictionary for Header Byte Offset from key.
	auto iterator = this->dataPointers_.find(hashKey);
	
	// If the data couldn't be found.
	if (iterator == this->dataPointers_.end())
	{
		std::cerr << "[DataBase] Error: Query not found." << std::endl;
		// Return empty string as a type of error.
		return "";
	}
	// If found then get Header Byte Offset.
	uint64_t headerOffset = iterator->second;
	
	// Search for data in BTree and return the result in string format.
	std::string result = this->dataStorage_.queryData(headerOffset);
	// If there was an issue while searching for data.
	if (result.empty())
	{
		std::cerr << "[DataBase] Error: Failed to retrieve data from BTree storage." << std::endl;
		// Return empty string as a type of error.
		return "";
	}
	
	// Print out console log, if successful.
	std::cout << "[DataBase] Data Query Processed: " << key 
		<< " | Hash Key: " << hashKey 
		<< " | Header Offset: " << headerOffset 
		<< " | Output: " << result << std::endl;

	// Return resulting data from search.
	return result;
}

/**
 * @brief Recieves JSON-like string, determines query type, processes Query.
 * @details Query processing offloaded to Strategy Design Pattern QueryType
 * interface.
 * @param rawQuery JSON-like string which is parsed for different query
 * processing.
 * @return QueryResponse with proper HTTP-style status codes and a response
 * body of Query.
 */
QueryResponse DataBase::handleQuery(std::string rawQuery)
{
	// Unpacks a raw JSON-like string in the following format:
	// {"query-type": ". . .", . . .}
	size_t formatPos = rawQuery.find("\"query-type\"");
	size_t colonPos = rawQuery.find(':', formatPos);
	size_t valStart = rawQuery.find('"', colonPos);
	size_t valEnd = rawQuery.find('"', valStart + 1);
	
	// If the format of the JSON doesn't match.
	if (formatPos == std::string::npos || valStart == std::string::npos || valEnd == std::string::npos)
	{
		std::cerr << "[DataBase] Error: Incorrect Query Format Received." << std::endl;
		// Create a QueryResponse, with statusCode 400, and error message.
		// statusCode 400 = improper data format.
		return {400, "{\"Error\": \"Incorrect Query Format Received.\"}"};
	}
	// Parse string to get the query type value.
	std::string queryType = rawQuery.substr(valStart + 1, valEnd - (valStart + 1));
	
	// Get all remaining chars after getting the query type.
	std::string remainingData = rawQuery.substr(valEnd + 1);
	
	// Create an QueryType interface pointer.
	QueryType* QT = nullptr;
	
	if (queryType == "search")
	{
		QT = new Search(remainingData); // Create the strategy pattern Search Object.
	} 
	else if (queryType == "write")
	{
		QT = new Write(remainingData); // Create the strategy pattern Write Object.
	}
	else // If the query type wasn't found.
	{
		std::cerr << "[DataBase] Error: Query Type Not Found." << std::endl;
		// Create a QueryResponse, with statusCode 400, and error message.
		// statusCode 400 = improper data format.
		return {400, "{\"Error\": \"Query Type Not Found \"}"};
	}
	
	// Execute the strategy design pattern on the QueryType and retrieve
	// the QueryResponse.
	QueryResponse result = QT->handleQuery();

	// Free memory from the interface.
	delete QT;
	// Return the QueryResponse after handling the query.
	return result;
}

/**
 * @brief Handles the creation and accessing of the DataBase Singleton.
 */
DataBase *DataBase::getDataBase()
{
	// Create a global/static instance of the DataBase Singleton.
	static DataBase instance;
	// Returns the instance by reference.
	return &instance;
}

/**
 * @brief Takes in a rawQuery, parses it, takes out a key, and stores it.
 * @details explicitly defines constructor to avoid default constructor
 * from being used in parent class.
 */
Search::Search(std::string rawQuery)
{
	// Parses through a query with the following format:
	// "key": ". . ." }
	size_t formatPos = rawQuery.find("\"key\"");
	size_t colonPos = rawQuery.find(':', formatPos);
	size_t valStart = rawQuery.find('"', colonPos);
	size_t valEnd = rawQuery.find('"', valStart + 1);
	
	// If the format of the rawQuery is incorrect, mark query as not valid.
	if (formatPos == std::string::npos || valStart == std::string::npos || valEnd == std::string::npos)
	{
		std::cerr << "[Search] Error: Incorrect Query Format Received." << std::endl;
		this->isValid_ = false;
	}
	else // If the format of JSON does match.
	{
		// Parse the key's value from the JSON.
		std::string key = rawQuery.substr(valStart + 1, valEnd - (valStart + 1));
		// Set the key attribute to the parsed key's value.
		this->key_ = key;
	}
}

/**
 * @brief Uses key_ to search DataBase.
 * @return QueryResponse data and status code.
 */
QueryResponse Search::handleQuery()
{
	// If the JSON-like input isn't formatted correctly
	if (!this->isValid_) 
	{
		// Create a QueryResponse, with statusCode 400, and error message.
		// statusCode 400 = improper data format.
		return {400, "{\"Error\": \"Incorrect Query Format Received.\"}"};
	}
	// Get DataBase singleton instance.
	DataBase* DB = DataBase::getDataBase();
	// Run a query in the DataBase using parsed key and return result.
	std::string result = DB->queryData(this->key_);
	if (result.empty()) // If the resulting string is empty (has an error).
	{
		// Create a QueryResponse, with statusCode 404, and error message.
		// statusCode 404 = data not found.
		return {404, "{\"Error\": \"Key not found\"}"};
	}
	// Return a QueryResponse with statusCode 200, and the resulting data.
	// statusCode 200 = query processed successfully.
	return {200, result};
}

/**
 * @brief Takes in a rawQuery, parses it, takes out a key and data,
 * and stores it.
 * @details explicitly defines constructor to avoid default constructor
 * from being used in parent class.
 */
Write::Write(std::string rawQuery)
{
	// Parses through a query with the following format:
	// "key": ". . .", "data": ". . ." }
	// This portion only parses the key part
	size_t formatPos = rawQuery.find("\"key\"");
	size_t colonPos = rawQuery.find(':', formatPos);
	size_t valStart = rawQuery.find('"', colonPos);
	size_t valEnd = rawQuery.find('"', valStart + 1);
	
	// If rawQuery's format is incorrect, mark query as not valid.
	if (formatPos == std::string::npos || valStart == std::string::npos || valEnd == std::string::npos)
	{
		std::cerr << "[Write] Error: Incorrect Query Format Received." << std::endl;
		isValid_ = false;
	}
	else
	{
		// Parse the key's value.
		std::string key = rawQuery.substr(valStart + 1, valEnd - (valStart + 1));
		// Set Write's key attribute to parsed key.
		this->key_ = key;
		
		// Parse remaining data string.
		formatPos = rawQuery.find("\"data\"");
		colonPos = rawQuery.find(':', formatPos);
		valStart = rawQuery.find('"', colonPos);
		// Set the end of data's value to not found, to account for
		// inputted data with escaped quotes \".
		// Note: Escaped quotes would break standard formatting.
		valEnd = std::string::npos;
		
		// If the start of the data does exist.
		if (valStart != std::string::npos)
		{
			// Iterate from the beginning passed the quote.
			for (size_t i = valStart + 1; i < rawQuery.size(); ++i)
			{
				// If we hit an escaped backslash, skip the next character
				if (rawQuery[i] == '\\' && i + 1 < rawQuery.size())
				{
					i++;
					continue;
				}
				// If we hit an unescaped quote, this is the real end
				if (rawQuery[i] == '"')
				{
					valEnd = i;
					break;
				}
			}
		}
		
		// If rawQuery's format is incorrect, mark query as not valid.
		if (formatPos == std::string::npos || valStart == std::string::npos || valEnd == std::string::npos)
		{
			std::cerr << "[Write] Error: Incorrect Query Format Received." << std::endl;
			isValid_ = false;
			return;
		}
		else
		{
			// Parse data's data from the rawQuery.
			std::string data = rawQuery.substr(valStart + 1, valEnd - (valStart + 1));
			// Set Write's attribute data to parsed data.
			this->data_ = data;
		}
	}
}

/**
 * @brief Writes data_ into the DataBase and adds key_ to the dataPointers_
 * dictionary.
 * @return QueryResponse data and status code.
 */
QueryResponse Write::handleQuery()
{
	// If the JSON-like input isn't formatted correctly
	if (!this->isValid_)
	{
		// Create a QueryResponse, with statusCode 400, and error message.
		// statusCode 400 = improper data format.
		return {400, "{\"Error\": \"Incorrect Query Format Received.\"}"};
	}
	// Get the DataBase's singleton instance.
	DataBase* DB = DataBase::getDataBase();
	// Attempt to write data to the DataBase using the parsed key and data,
	// and get if it succeeded.
	bool success = DB->writeData(this->key_, this->data_);
	if (success == false) // If it failed.
	{
		std::cerr << "[Write] Error: Failed to write data to storage engine." << std::endl;
		// Create a QueryResponse, with statusCode 500, and error message.
		// statusCode 500 = DataBase was unable to write.
		return {500, "{\"Error\": \"Database write failed\"}"};
	}
	// Create a QueryResponse, with statusCode 200 and a confirmation message.
	// statusCode 200 = DataBase was unable to write.
	return {200, "{\"Status\": \"Success\"}"};
}
