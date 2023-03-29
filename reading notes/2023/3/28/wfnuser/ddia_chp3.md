-  
  > Wer Ordnung hält, ist nur zu faul zum Suchen.  
  (If you keep things tidily ordered, you’re just too lazy to go searching.)  
  —German proverb  

- Database needs to do two things:  
	- store & retrieve  
- Why you care about how the database handles storage and retrieval internally?  
	- Have a rough idea of what the storage engine is doing under the hood -> Tune a storage engine  
- ## Data Structures That Power Your Database  
	-  
	  ``` bash
	  	  	#!/bin/bash
	  	      db_set () {
	  	          echo "$1,$2" >> database
	  	  	}
	  	      db_get () {
	  	          grep "^$1," database | sed -e "s/^$1,//" | tail -n 1
	  	  	}
	  	      
	  	      $ db_set 123456 '{"name":"London","attractions":["Big Ben","London Eye"]}' 
	  	      $ db_set 42 '{"name":"San Francisco","attractions":["Golden Gate Bridge"]}'
	  	  	$ db_get 42
	  	  	{"name":"San Francisco","attractions":["Golden Gate Bridge"]}
	  	      
	  	      $ cat database
	  	  	123456,{"name":"London","attractions":["Big Ben","London Eye"]} 
	  	      42,{"name":"San Francisco","attractions":["Golden Gate Bridge"]} 
	  	      42,{"name":"San Francisco","attractions":["Exploratorium"]}
	  	  
	  ```
	- appending to a file is generally very efficient -> db_set perform well  
		- many databases internally use a log, which is an append-only data file.  
	- db_get has terrible performance  
		- In algorithmic terms, the cost of a lookup is O(n)  
	- we need index  
		- Maintaining additional structures incurs overhead, especially on writes.  
	- an important trade-off in storage systems:  
		- well-chosen indexes speed up read queries, but every index slows down writes.  