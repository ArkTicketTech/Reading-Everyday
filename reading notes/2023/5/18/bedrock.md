[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
0x00007fefcd118d7f in __GI___poll (fds=0x563c7a7778e0, nfds=2, timeout=-1) at ../sysdeps/unix/sysv/linux/poll.c:29
29        ../sysdeps/unix/sysv/linux/poll.c: No such file or directory.
(gdb) b PROFILING::set_query_source
Breakpoint 1 at 0x563c70b9fa7f: file /home/fu/mysql-8.0.32/sql/sql_profile.cc, line 484.
(gdb) n
[Switching to Thread 0x7fef806f9640 (LWP 55791)]

Thread 40 "connection" hit Breakpoint 1, PROFILING::set_query_source (this=0x7fef582a9dc0, query_source_arg=0x7fef581ce870 "select sbtest1.k from sbtest1 join sbtest2 on sbtest1.c=sbtest2.c", query_length_arg=65) at /home/fu/mysql-8.0.32/sql/sql_profile.cc:484
484                                         size_t query_length_arg) {
(gdb) p *query_source_arg
$1 = 115 's'
(gdb) p query_length_arg
$2 = 65
(gdb) p query_source_arg
$3 = 0x7fef581ce870 "select sbtest1.k from sbtest1 join sbtest2 on sbtest1.c=sbtest2.c"
(gdb) n
485          DBUG_TRACE;
(gdb) n
487          if (!enabled) return;
(gdb) p enabled
$4 = false
(gdb) n
dispatch_command (thd=0x7fef580081b0, com_data=0x7fef806f8340, command=COM_QUERY) at /home/fu/mysql-8.0.32/sql/sql_parse.cc:2013
2013              const LEX_CSTRING orig_query = thd->query();
(gdb) bt
#0  dispatch_command (thd=0x7fef580081b0, com_data=0x7fef806f8340, command=COM_QUERY)
    at /home/fu/mysql-8.0.32/sql/sql_parse.cc:2013
#1  0x0000563c70b44c4b in do_command (thd=0x7fef580081b0) at /home/fu/mysql-8.0.32/sql/sql_parse.cc:1439
#2  0x0000563c70d913f7 in handle_connection (arg=0x563c7aa9fa40)
    at /home/fu/mysql-8.0.32/sql/conn_handler/connection_handler_per_thread.cc:302
#3  0x0000563c72d6ec28 in pfs_spawn_thread (arg=0x563c7aa9aee0) at /home/fu/mysql-8.0.32/storage/perfschema/pfs.cc:2986
#4  0x00007fefcd094b43 in start_thread (arg=<optimized out>) at ./nptl/pthread_create.c:442
#5  0x00007fefcd126a00 in clone3 () at ../sysdeps/unix/sysv/linux/x86_64/clone3.S:81
(gdb) p thd
