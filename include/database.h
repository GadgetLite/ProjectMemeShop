#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <cstdint>

/**
 * @struct Header
 * @brief Formatting struct used for pulling data out from a file and reading it.
 * @details Default state of the struct is corrupted using UINT64_MAX. When overwritten 
 * successfully, the data is no longer considered corrupted.
 */
struct Header
{
		// Get the raw byte offset of the starting position of where a data block starts.
		uint64_t startPosition = UINT64_MAX;
		// Get the raw byte offset of the ending position of where a data block ends.
		uint64_t endPosition = UINT64_MAX;
		
		/**
		 * @brief Opens the header file, jumps to offset, and reads data into itself.
		 * @param headerFilePath - Path where the Header File is located.
		 * @param byteOffset - Byte offset of where header is located.
		 * @return true if header had data from file read into it 
		 * and file bounds are >= sizeof(Header), false otherwise.
		 */
		bool getData(const std::string& headerFilePath, uint64_t byteOffset);
		
		/**
		 * @brief Checks if the header was able to successfully populate data into it.
		 * @details By default, data is corrupted because nothing has been populated.
		 * If the method getData was run and no data was read, the data is considered
		 * corrupted.
		 * @return true if all attributes != UINT64_MAX, false otherwise.
		 */
		bool isCorrupted();
};

/**
 * @class BTree
 * @brief Writes and Reads data from files.
 * @details The Header File has headers with the same format as the Header struct,
 * which point to the start and end of data within the Data File. Writing appends a
 * header to the Header File and appends the data to the Data File. Reading finds
 * the header and goes to the position within the Data File and returns it.
 * BTree has no data preference and can accept any data.
 */
class BTree
{
	private:
		// File path for the Header File where formatted headers are stored.
		std::string headerFilePath_;
		// File path for the Data File where data is stored.
		std::string dataFilePath_;
		
		// File size of Header File.
		uint64_t headerFileSize_;
		// File size of Data File.
		uint64_t dataFileSize_;
		
		/**
		 * @brief Checks if previous header exists.
		 * @param header Takes a header formatter as a pointer.
		 * @return Header* with formatted data if read successfully,
		 * Header* with default values if truncation occured, 
		 * Header* with default values if read was unsuccessful,
		 * nullptr if file was empty.
		 */
		Header* getPrevHeader(Header *header);
	public:
		/**
		 * @brief Creates Data File and Header File and gets file size.
		 * @param headerFilePath File path for the Header File.
		 * @param dataFilePath File path for Data File.
		 */
		BTree(const std::string& headerFilePath, const std::string& dataFilePath);
		
		/**
		 * @brief Creates/appends a header for Header File, writes 
		 * inputted data to Data File.
		 * @param data Takes inputted data to be written in the Data File.
		 * @return uint64_t Byte Offset of where the header was written in
		 * Header File.
		 */
		uint64_t writeData(const std::string& data);

		/**
		 * @brief Searches/queries and returns data based on header offset.
		 * @param offset Byte Offset of header in Header File.
		 * @return std::string Data stored within the Data File in specified
		 * location.
		 */
		std::string queryData(uint64_t offset);
};
/**
 * @struct QueryResponse
 * @brief Acts as a go-between for each of the types of database queries,
 * with an HTTP-style status code and returned data.
 */
struct QueryResponse 
{
	// Takes in HTTP-style error codes for web servers to interpret.
	int statusCode;
	// Takes in returned data from a given method.
	std::string body;
};

/**
 * @class DataBase
 * @brief A Singleton DataBase class which handles JSON-like queries
 * for Reading and Writing data.
 * @details Stores references to data in Key-Value pairs and returns
 * QueryResponses for every QueryType that there is.
 */
class DataBase
{
	private:
		// Dictionary with Key-Value pairs of hash to Header File Byte Offsets.
		std::unordered_map<uint64_t, uint64_t> dataPointers_;
		// File path that stores dataPointers_ data between sessions.
		std::string mapFilePath_;
		// Multi-threading control.
		std::mutex mutex_;
		// Create a BTree for Reading and Writing data.
		BTree dataStorage_;
		
		/**
		 * @brief Creates/opens Map File and attempts to initialize 
		 * dataPointers_ with previous data.
		 */
		DataBase();
		
		/**
		 * @brief Keeps destructor untouchable for the Singleton Pattern.
		 * @details Acts as a default destructor.
		 */
		~DataBase() {};
		
		/**
		 * @brief Generate a hash for a given Key.
		 * @details Will not perform hashing for Keys which size is less than or equal 
		 * to 8 chars, but will only convert it to a uint64_t.
		 * @param inputted Takes an inputted string and converts it to a uint64_t hash.
		 * @return uint64_t Final hash value of the inputted string.
		 */
		uint64_t stringToHash(const std::string& input);
	public:
		// Remove deep copy feature.
		DataBase(DataBase &other) = delete;
		// Remove soft copy feature.
		void operator=(const DataBase&) = delete;
		
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
		bool writeData(const std::string& key, const std::string& data);
		
		/**
		 * @brief Takes a Key, checks if there is a header, and attempts to return data.
		 * @details Key is converted into a uin64_t hash via stringToHash. Searches for
		 * Key-Value pair in dataPointers_ where value is the Byte Offset to the header.
		 * Attempts to read header from Header File and retrieve data from Data File.
		 * @param key Used for finding data in dataPointers_.
		 * @return std::string Data if data retrieved successfully retrieved from BTree.
		 * empty string otherwise.
		 */
		std::string queryData(const std::string& key);
		
		/**
		 * @brief Handles the creation and accessing of the DataBase Singleton.
		 */
		static DataBase* getDataBase();

		/**
		 * @brief Recieves JSON-like string, determines query type, processes Query.	 
		 * @details Query processing offloaded to Strategy Design Pattern QueryType
		 * interface.
		 * @param rawQuery JSON-like string which is parsed for different query 
		 * processing.
		 * @return QueryResponse with proper HTTP-style status codes and a response
		 * body of Query.
		 */
		QueryResponse handleQuery(std::string);
};

/**
 * @class QueryType
 * @brief A virtual class which allows children to define how they handle
 * certain query types.
 * @details Follows the Strategy Design Pattern for breaking down certain
 * issues into objects which can define solutions for that problem.
 */
class QueryType
{
	public:
		// Allows polymorphic classes to uninitialize without errors.
		virtual ~QueryType() = default;
		// Creates the Strategy Design Pattern of implementing a 
		// method in a child class.
		virtual QueryResponse handleQuery() = 0;
};

/**
 * @class Search
 * @brief Handles the Search QueryType by searching DataBase with a Key.
 * @details Parses a Key out of an inputted rawQuery and stores it as an
 * attribute.
 */
class Search : public QueryType
{
	private:
		// A Key used in searching the DataBase. Defined by parsing rawQuery.
		std::string key_;
		bool isValid_ = true;
	public:
		/**
		 * @brief Takes in a rawQuery, parses it, takes out a key, and stores it. 
		 * @details explicitly defines constructor to avoid default constructor
		 * from being used in parent class.
		 */
		explicit Search(std::string rawQuery);

		/**
		 * @brief Uses key_ to search DataBase. 
		 * @return QueryResponse data and status code.
		 */
		QueryResponse handleQuery();
};

/**
 * @class Write
 * @brief Handles the Write QueryType by adding a Key and writing Data to the 
 * DataBase.
 * @details Parses a Key and data out of an inputted rawQuery and stores it 
 * as attributes.
 */
class Write : public QueryType
{
	private:
		// A Key used in searching the DataBase. Defined by parsing rawQuery.
		std::string key_;
		// Data to be stored/written to the DataBase. Defined by parsing rawQuery.
		std::string data_;
		// Checks to see if rawQuery is in a valid format.
		bool isValid_ = true;
	public:
		/**
		 * @brief Takes in a rawQuery, parses it, takes out a key and data, 
		 * and stores it.
		 * @details explicitly defines constructor to avoid default constructor
		 * from being used in parent class.
		 */
		explicit Write(std::string rawQuery);
		
		/**
		 * @brief Writes data_ into the DataBase and adds key_ to the dataPointers_
		 * dictionary. 
		 * @return QueryResponse data and status code.
		 */
		QueryResponse handleQuery();
};

#endif
