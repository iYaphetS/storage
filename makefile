
CC=gcc
CPPFLAGS=-I ./include -I/usr/include/fastdfs -I/usr/include/fastcommon -I/usr/local/include/hiredis/
CFLAGS=-Wall
LIBS=-lfdfsclient -lfastcommon -lhiredis -lfcgi -lm

test_fdfs_api=./test/test_fdfs_api
test_redis_op=./test/test_redis_op
demo_cgi=./test/demo_cgi
echo_cgi=./test/echo_cgi
upload_cgi=./test/upload_cgi
data_cgi=./test/data_cgi
savefile_2_redis=./test/savefile_2_redis
#select_files=./test/select_files

target=$(test_fdfs_api) $(test_redis_op) $(demo_cgi) \
		$(savefile_2_redis)  $(echo_cgi) \
		$(upload_cgi) $(data_cgi)

ALL:$(target)


%.o:%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS)

#测试cgi显示上传的文件
$(data_cgi):./test/data_cgi.o make_log.o redis_op.o fdfs_api.o select_file.o cJSON.o util.o
	$(CC) $^ -o $@ $(LIBS)

#测试cgi上传文件
$(upload_cgi):./test/upload_cgi.o make_log.o util.o upload_file.o redis_op.o fdfs_api.o
	$(CC) $^ -o $@ $(LIBS)

#测试cgi自定义网页回显
$(echo_cgi):./test/echo_cgi.o 
	$(CC) $^ -o $@ $(LIBS)

#测试cgi自定义网页
$(demo_cgi):./test/demo_cgi.o 
	$(CC) $^ -o $@ $(LIBS)

#测试文件信息存储redis数据库操作
$(savefile_2_redis):./test/savefile_2_redis.o redis_op.o make_log.o fdfs_api.o
	$(CC) $^ -o $@ $(LIBS)

#测试从redis数据库读数据操作
#$(select_files):./test/select_files.o redis_op.o make_log.o 
#	$(CC) $^ -o $@ $(LIBS)

#测试fdfs上传下载文件操作
$(test_fdfs_api):./test/test_fdfs_api.o fdfs_api.o make_log.o 
	$(CC) $^ -o $@ $(LIBS)

#测试redis数据库操作
$(test_redis_op):./test/test_redis_op.o make_log.o redis_op.o 
	$(CC) $^ -o $@  $(LIBS)

clean:
	-rm -rf ./*.o $(target) ./test/*.o
distclean:
	-rm -rf ./*.o $(target) ./test/*.o
.PHONY: clean ALL distclean
