#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/socket.h>
#include <sys/mbuf.h>
#include <net/if.h>
#include <net/pfil.h>
#include <sys/uio.h>
#include <sys/conf.h>
#include <sys/ioccom.h>

static int bytes = 0;
static char* byteBuffer = NULL;
static int count = 0;
static struct cdev* sdev;

static struct cdevsw filter = {
	.d_version = D_VERSION,
	.d_open = open,
	.d_close = close,
	.d_read = read.
	.d_name = "filter"
};

int open(struct cdev* dev, int flag, int otyp, struct thread* td){

	int err = 0;
	if(count > 0){
		return EBUSY;
	}
	count = 1;
	return err;

} 

int close(struct cdev* dev, int fflag, int devtype, struct thread* td){

	int err = 0;
	count = 0;
	return err;

}

int read(struct cdev* dev, struct uio* uio, int ioflag){

	int size = 0;
	sprintf(byteBuffer, "%016d,%016d\n", bytes);
	size = uiomove(byteBuffer, MIN(uio-uio_resid, 
strlen(byteBuffer)), uio);
	return size;

}

static int packetFilter(void* arg, struct mbuf** m, struct ifnet* ifp, 
int dir, struct inpcb* inp){

	bytes += (*m)->m_len;
	uprintf("This is a test:%d", bytes);
	return 0;

}

static int deinitializeFilter(void){
	struct pfil_head* ptrhead;
	ptrhead = pfil_head_get(PFIL_TYPE_AF, AF_INET);
	pfil_remove_hook(packetFilter, NULL, PFIL_IN, ptrhead);
	return 0;
}

static int initializeFilter(void){

	struct pfil_head* ptrhead;
	ptrhead = pfil_head_get(PFIL_TYPE_AF, AF_INET);
	if(ptrhead == NULL){
		return ENOENT;
	}
	pfil_add_hook(packetFilter, NULL, PFIL_IN, ptrhead);
		return 0;
}



static int eventHandler(struct module *module, int event, void* addInfo){

	int status = 0;
	switch(event){
	  case MOD_LOAD: 
		status = initializeFilter(); 
		break;
	  case MOD_UNLOAD:
		status = deinitializeFilter();
		break;
	}
	return status;
}


static moduledata_t data = {
	"pfil_hook",
	eventHandler,
	NULL
};

DECLARE_MODULE(pfil_hook, data, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
