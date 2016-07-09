all:
	make -f dev_udp_test.mk
	#make -f dev_event_test.mk
clean:
	make -f dev_udp_test.mk clean
	#make -f dev_event_test.mk clean