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
					return Response.json({ error: "Missing 'SQL' statement" }, { status: 400 });
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

				return Response.json({ success: true, result }, { status: 200, headers: { "Content-Type": "application/json" }});
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
