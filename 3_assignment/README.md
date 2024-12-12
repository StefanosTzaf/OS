
The main shared memory struct consists of sub-structs to enhance modularity and clean code.


First sub-struct is about statistics : 

Second substruct is about circular waiting buffer that ensure FCFS policy for the first 50
visitors. If that buffer did not existed fcfs could not be assured while one semafore just suspend processes and wake them up randomly
Now with one semaphore for every buffer position we know who came first ... NOT DYNAMICALLY WE HAVE TO DEFINED IT BEFORE

!!!!!!!!In main struct i have a semaphore too, to ensure that visitors that exced max_visitors are suspended there (without fcfs in that case)
Initialized to max_visitors and if this is exceded (if buffer back and front is full)

