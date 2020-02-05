# Redis Native: Producer/Consumer Performance Testing

Complexity
```
LREN    - O(N), N
LRANGE  - O (S + N)
LINDEX  - O (N)
LINSET  - O (N)
LSET    - O (N), O (1) for header and tail
LTRIM   - O (N)
```

Connection
```c
void connection() {
	struct timeval timeout = { 2, 500000 }; // 2.5 segundos
	c = redisConnectWithTimeout(hostname, port, timeout);
	if (c == NULL || c->err) {
		if (c) {
			printf("Connection error: %s\n", c->errstr);
			redisFree(c);
		}
		else 
			printf("Connection error: can't allocate redis context\n");
		
		exit(1);
	}
}
```

Runnable
```c
void threadRun(_In_ _beginthread_proc_type _StartAddress) {
	HANDLE hThread;
	hThread = (HANDLE)_beginthread(_StartAddress, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
}
```

Producer
```c
void producer(void *param) {
	unsigned int j = 0;

	while (true) {
		clearScreen("PRODUCER \n");

		float aux = 0;
		for (int i = 0; i < 150; i++) {
			aux = _num++;
			reply = (redisReply*)redisCommand(context, "RPUSH list element-%lld", aux);		
			freeReplyObject(reply);
		}
   
		printf("RPUSH list element-%lld\n", aux);
		reply = (redisReply*)redisCommand(context, "LLEN list");
		printf("LLEN list: %lld\n",reply->integer);
		freeReplyObject(reply);

		reply = (redisReply*)redisCommand(context, "LRANGE list 0 -1");
		freeReplyObject(reply);
		Sleep(15);
	}

	free(params);
}
```

Consumer
```c
void consumer(void *param) {
	
	while (true) {
		clearScreen("CONSUMER \n");

		reply = (redisReply*)redisCommand(context, "LLEN list");
		unsigned int size = reply->integer;
		printf("LLEN list: %lld\n", size);
		freeReplyObject(reply);
		for (int i = 0; i < 100 ; i++) {
			reply = (redisReply*)redisCommand(context, "BLPOP list 0");
			freeReplyObject(reply);
		}

		Sleep(13);
	}
	
	free(params);
}
```

Laptop
```
Macbook (2014) w/ Bootcamp
Intel(R) Core(TM) i5-3210M
CPU @ 2.50GHz 2.49GHz
16 GB RAM
System x64
```

## References
 * [Redis Native Producer Consumer](https://github.com/vicboma1/Redis-Native-Producer-Consumer)
 * [Bufferedis - Asynchronous Fork](https://github.com/vicboma1/bufferedis)
 
## Video
*  [Redis Native (Hiredis):  Producer Consumer Performance Testing | Visual Studio C/C++](https://youtu.be/3aRJe4E3X6w)
