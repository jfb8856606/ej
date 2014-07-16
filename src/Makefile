# MODNAME
MODNAME = ej

#CC: gcc / cc
CC = gcc

# CFLAGS
CFLAGS += -g

# LDFLAGS
# LDFLAGS += -lpthread  -lrt \
# 		-L$(LIBPATH) \
# 		-lethdev -lrte_cmdline -lrte_eal -lrte_hash -lrte_kni -lrte_kvargs \
# 		-lrte_lpm -lrte_malloc -lrte_mbuf -lrte_mempool -lrte_meter \
# 		-lrte_pmd_e1000 -lrte_pmd_ixgbe -lrte_pmd_ring -lrte_pmd_virtio \
# 		-lrte_pmd_vmxnet3 -lrte_power -lrte_ring -lrte_sched -lrte_timer -ldb -ldl

OBJS = ej.o config.o file.o net.o

#==============================

$(MODNAME):$(OBJS)
# 	$(CC) -o $(MODNAME) $(OBJS) $(LDFLAGS) 
	$(CC) -o $(MODNAME) $(OBJS) 

clean:
	rm -rf *.o
	rm -rf $(MODNAME)