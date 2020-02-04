#include "pch.h"
#include <iostream>
#include <Windows.h>
#include "hiredis.h"
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <process.h>

//http://antsnote.club/2019/02/16/Redis-Redis%E5%88%97%E8%A1%A8/

constexpr char *hostname = "127.0.0.1";
constexpr int port = 6379;

redisReply *reply;
redisContext *context;

static LONGLONG _num = 0;

void clearScreen(const char* str) {
	system("@cls||clear");
	printf("%s \n", str);
}

void printHelp() {
	printf("\nArgs: Redis {producer or consumer}");
}

void printArguments(int argc, char **argv) {
	printf("Have %d arguments: \n", argc);
	for (int i = 0; i < argc; ++i)
		printf("[%d]=%s\n", i, argv[i]);
}

void producer(void *params) {
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

void consumer(void *params) {

	while (true) {
		clearScreen("CONSUMER \n");

		reply = (redisReply*)redisCommand(context, "LLEN list");
		unsigned int size = reply->integer;
		printf("LLEN list: %lld\n", size);
		freeReplyObject(reply);
		for (int i = 0; i < 100 && (size >= 100); i++) {
			reply = (redisReply*)redisCommand(context, "BLPOP list 0");
			freeReplyObject(reply);
		}

		Sleep(13);
	}
	
	free(params);
}

void  connection() {
	struct timeval timeout = { 2, 500000 }; // 2.5 segundos
	context = redisConnectWithTimeout(hostname, port, timeout);
	if (context == NULL || context->err) {
		if (context) {
			printf("Connection error: %s\n", context->errstr);
			redisFree(context);
		}
		else 
			printf("Connection error: can't allocate redis context\n");
		
		exit(1);
	}
}

void threadRun(_In_ _beginthread_proc_type _StartAddress) {
	HANDLE hThread;
	hThread = (HANDLE)_beginthread(_StartAddress, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
}

int main(int argc, char **argv) {

	printArguments(argc,argv);
	connection();

	if (argc == 2) {
		if (strcmp(argv[1], "producer") == 0)
			threadRun(producer);
		else 
			if (strcmp(argv[1], "consumer") == 0) 
				threadRun(consumer);
	}
	else
		if (argc == 1) {

			clearScreen("REDIS -> EXAMPLE.C \n");

			// Example.c
			reply = (redisReply*)redisCommand(context, "PING");
			printf("PING: %s\n", reply->str);
			freeReplyObject(reply);

			reply = (redisReply*)redisCommand(context, "SET %s %s", "foo", "hello world");
			printf("SET: %s\n", reply->str);
			freeReplyObject(reply);

			reply = (redisReply*)redisCommand(context, "SET %b %b", "bar", (size_t)3, "hello", (size_t)5);
			printf("SET (binary API): %s\n", reply->str);
			freeReplyObject(reply);

			reply = (redisReply*)redisCommand(context, "GET foo");
			printf("GET foo: %s\n", reply->str);
			freeReplyObject(reply);

			reply = (redisReply*)redisCommand(context, "GET bar");
			printf("GET bar: %s\n", reply->str);
			freeReplyObject(reply);

			reply = (redisReply*)redisCommand(context, "GET hello");
			printf("GET hello: %s\n", reply->str);
			freeReplyObject(reply);

			reply = (redisReply*)redisCommand(context, "INCR counter");
			printf("INCR counter: %lld\n", reply->integer);
			freeReplyObject(reply);

			reply = (redisReply*)redisCommand(context, "INCR counter");
			printf("INCR counter: %lld\n", reply->integer);
			freeReplyObject(reply);

			reply = (redisReply*)redisCommand(context, "DEL mylist");
			printf("DEL mylist: %lld\n", reply->elements);
			freeReplyObject(reply);
			for (unsigned int j = 0; j < 10; j++) {
				char buf[64];

				snprintf(buf, 64, "%d", j);
				printf("mylist LPUSH element[%lu]-%s\n", j, buf);
				reply = (redisReply*)redisCommand(context, "LPUSH mylist element-%s", buf);
				freeReplyObject(reply);
			}

			reply = (redisReply*)redisCommand(context, "LRANGE mylist 0 -1");
			if (reply->type == REDIS_REPLY_ARRAY) {
				for (unsigned int j = 0; j < reply->elements; j++) {
					printf("%u) %s\n", j, reply->element[j]->str);
				}
			}
			freeReplyObject(reply);

			redisFree(context);
			
		}
	else 
			printHelp();
	
	return 0;
}