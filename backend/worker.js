export default 
{
	async fetch(request, env, ctx) 
	{
		const url = new URL(request.url);
		const path = url.pathname;

		// Get API key from the header.
		const apiKey = request.headers.get("X-API-Key");
		// Verify API key before processing SQL Statement.
		if (!apiKey || apiKey !== env.API_KEY) 
		{
			// Return a status error code 401 (authorization) and an error body. 
			return new Response("Unauthorized", { status: 401 });
		}

		try {
			// Receive a POST request from /api/sql.
			if (request.method === "POST" && path === "/api/sql") 
			{
				// Get the post body.
				const body = await request.json();
				const { sql, params = [] } = body; // Set SQL statement and params as values from the request body.
				
				if (!sql) // If SQL statement wasn't provided.
				{
					// Return a status error code 400 (formatting issue) and an error body. 
					return Response.json({ success: false, error: "Missing 'SQL' statement" }, { status: 400 });
				}
				
				// Make the raw SQL string into a prepared statement to be sent to database.
				let statement = env.DB.prepare(sql);
				if (params.length > 0) 
				{
					// Parse through statement, replacing all '?' placeholders with inputted parameters.
					statement = await statement.bind(...params); 
				}
				// .all() executes the query in the database.				
				const result = await statement.all();
				
				// Return result and status code 200 (query success).
				return Response.json({ success: true, result }, { status: 200, headers: { "Content-Type": "application/json" }});
			}
			else if (request.method === "PUT" && path.startsWith("/api/upload/"))
			{
				// Remove the /api/upload from the url to parse the Key for uploaded data.
				const key = path.replace("/api/upload/", "");

				if (!key)
				{
					// Return a status error code 400 (formatting issue) and an error body. 
					return Response.json({ success: false, error: "Missing file key." }, { status: 400, headers: { "Content-Type": "application/json" }});
				}

				// Get provided Content Type.
				const contentType = request.headers.get("Content-Type");

				if (!contentType) // If Content Type wasn't provided.
				{
					// Return a status error code 400 (formatting issue) and an error body. 
					return Response.json({ success: false, key, error: "Missing Content-Type" }, { status: 400, headers: { "Content-Type": "application/json" }});
				}
				// Executes the put request in the database with the Key-Value pairs alongside the content type.
				await env.BUCKET.put(key, request.body, {
					httpMetadata: { contentType }
				});
				
				// Return result and status code 200 (query success).
				return Response.json({ success: true, key }, { status: 200, headers: { "Content-Type": "application/json" }});
			}
			else if (request.method === "GET" && path.startsWith("/api/blob/"))
			{
				// Remove the /api/blob from the url to parse the Key for uploaded data.
				const key = path.replace("/api/blob/", "");
				if (!key)
				{
					// Return a status error code 400 (formatting issue) and an error body. 
					return Response.json({ success: false, error: "Missing file key." }, { status: 400, headers: { "Content-Type": "application/json" }});
				}
				
				// Retrieve object from bucket using Key.
				const object = await env.BUCKET.get(key);
				if (!object)
				{
					// Return a status error code 404 (data not found) and an error body. 
					return Response.json({ success: false, error: "Image not found." }, { status: 404, headers: { "Content-Type": "application/json" }});
				}
				// Create a header and write the file/data format from metadata.
				const headers = new Headers();
				object.writeHttpMetadata(headers);
				headers.set("etag", object.httpEtag);
				
				// Return result and status code 200 (query success).
				return new Response(object.body, { status: 200, headers });
			}
			else if (request.method === "DELETE" && path.startsWith("/api/blob/"))
			{
				// Remove the /api/blob from the url to parse the Key for uploaded data.
				const key = path.replace("/api/blob/", "");
				if (!key)
				{
					// Return a status error code 400 (formatting issue) and an error body. 
					return Response.json({ success: false, error: "Missing file key." }, { status: 400, headers: { "Content-Type": "application/json" }});
				}
				
				try
				{
					// Delete object from bucket using Key.
					await env.BUCKET.delete(key);

					return Response.json({ success: true, deletedKey: key }, { status: 200, headers: { "Content-Type": "application/json" }});
				}
				catch (err)
				{
					// Return a status error code 500 (server error) and an error body. 
					return Response.json({ success: false, error: err.message }, { status: 500, headers: { "Content-Type": "application/json" }});
				}
			}
			
			// Return a status error code 404 (page not found) and an error body. 
			return new Response("Page Not Found", { status: 404 });

		} 
		catch (err) // If anything fails
		{
			// Return a status error code 500 (server error) and an error body. 
			return Response.json({ error: err.message }, { status: 500, headers: { "Content-Type": "application/json" }});
		}
	}
};
