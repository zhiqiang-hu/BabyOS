#include "modules/inc/b_mod_netif/b_mod_trans.h"

#if (defined(_NETIF_ENABLE) && (_NETIF_ENABLE == 1))

#if (defined(VENDOR_UBUNTU) && (VENDOR_UBUNTU == 1))

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils/inc/b_util_list.h"
#include "utils/inc/b_util_log.h"

static LIST_HEAD(socket_list_head);

typedef struct
{
    int              sockfd;
    void            *cb_arg;
    pbTransCb_t      callback;
    uint8_t          readable;
    uint8_t          writeable;
    bFIFO_Info_t     rx_fifo;
    uint16_t         local_port;
    uint16_t         remote_port;
    uint32_t         remote_ip;
    bTransType_t     type;
    pthread_t        thread;
    struct list_head list;
} bTrans_t;

void *thread_function(void *arg)
{
    uint8_t           buf[CONNECT_RECVBUF_MAX];
    struct list_head *pos    = NULL;
    bTrans_t         *ptrans = NULL;
    int               retval = -1;
    while (1)
    {
        list_for_each(pos, &socket_list_head)
        {
            ptrans = (bTrans_t *)list_entry(pos, bTrans_t, list);
            if ((retval = recv(ptrans->sockfd, buf, sizeof(buf), 0)) == -1)
            {
                printf("Error: failed to receive data from server\r\n");
            }
            else if (retval > 0)
            {
                printf("recv:%d ...\r\n", retval);
                bFIFO_Write(&ptrans->rx_fifo, buf, retval);
                ptrans->readable = 1;
                ptrans->callback(B_TRANS_NEW_DATA, ptrans, ptrans->cb_arg);
            }
        }
        sleep(1);
    }
    return NULL;
}

int bSocket(bTransType_t type, pbTransCb_t cb, void *user_data)
{
    static uint8_t init_f = 0;
    pthread_t      thread;
    int            result;

    if (init_f == 0)
    {
        // 创建线程
        result = pthread_create(&thread, NULL, thread_function, NULL);
        if (result != 0)
        {
            printf("Thread creation failed\r\n");
            return -1;
        }
        init_f = 1;
    }

    if (cb == NULL || (type != B_TRANS_CONN_TCP && type != B_TRANS_CONN_UDP))
    {
        return -1;
    }
    int sockfd = -1;
    sockfd     = socket(AF_INET, (type == B_TRANS_CONN_TCP) ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        b_log_e("Error: socket creation failed\r\n");
        return -1;
    }

    bTrans_t *ptrans = (bTrans_t *)malloc(sizeof(bTrans_t));
    if (ptrans == NULL)
    {
        close(sockfd);
        return -2;
    }
    memset(ptrans, 0, sizeof(bTrans_t));
    uint8_t *pbuf = (uint8_t *)malloc(CONNECT_RECVBUF_MAX);
    if (pbuf == NULL)
    {
        free(ptrans);
        ptrans = NULL;
        return -2;
    }
    bFIFO_Init(&ptrans->rx_fifo, pbuf, CONNECT_RECVBUF_MAX);
    ptrans->sockfd    = sockfd;
    ptrans->type      = type;
    ptrans->callback  = cb;
    ptrans->cb_arg    = user_data;
    ptrans->readable  = 0;
    ptrans->writeable = 0;
    list_add(&ptrans->list, &socket_list_head);
    return (int)ptrans;
}

int bConnect(int sockfd, char *remote, uint16_t port)
{
    if (sockfd < 0 || remote == NULL)
    {
        return -1;
    }
    bTrans_t *ptrans = (bTrans_t *)sockfd;

    struct hostent  *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(remote)) == NULL)
    {
        printf("gethostbyname fail\r\n");
        return -1;
    }
    addr_list = (struct in_addr **)he->h_addr_list;
    struct sockaddr_in serverAddr;
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_port        = htons(port);
    serverAddr.sin_addr.s_addr = addr_list[0]->s_addr;

    // 连接到服务器
    if (connect(ptrans->sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        printf("Error: connection failed\r\n");
        ptrans->callback(B_TRANS_ERROR, ptrans, ptrans->cb_arg);
        return -1;
    }
    ptrans->writeable = 1;
    ptrans->callback(B_TRANS_CONNECTED, ptrans, ptrans->cb_arg);
    printf("Connected to the server.\r\n");
    return 0;
}

int bBind(int sockfd, uint16_t port)
{
    return -1;
}

int bListen(int sockfd, int backlog)
{
    return -1;
}

int bRecv(int sockfd, uint8_t *pbuf, uint16_t buf_len, uint16_t *rlen)
{
    int      read_len = 0;
    uint16_t fifo_len = 0;
    if (sockfd < 0 || pbuf == NULL || buf_len == 0)
    {
        return -1;
    }
    bTrans_t *ptrans = (bTrans_t *)sockfd;
    read_len         = bFIFO_Read(&ptrans->rx_fifo, pbuf, buf_len);
    if (read_len < 0)
    {
        return -2;
    }
    if (rlen)
    {
        *rlen = (uint16_t)(read_len & 0xffff);
    }
    bFIFO_Length(&ptrans->rx_fifo, &fifo_len);
    if (fifo_len == 0)
    {
        ptrans->readable = 0;
    }
    return 0;
}

int bSend(int sockfd, uint8_t *pbuf, uint16_t buf_len, uint16_t *wlen)
{
    uint16_t writeable_len = 0;
    int      retval        = -1;
    if (sockfd < 0 || pbuf == NULL || buf_len == 0)
    {
        return -1;
    }
    bTrans_t *ptrans = (bTrans_t *)sockfd;
    retval           = send(ptrans->sockfd, pbuf, buf_len, 0);
    if (retval < 0)
    {
        return -1;
    }
    printf("send %d \r\n", retval);
    if (retval >= 0 && wlen != NULL)
    {
        *wlen = retval;
    }
    return 0;
}

uint8_t bSockIsReadable(int sockfd)
{
    if (sockfd < 0)
    {
        return 0;
    }
    bTrans_t *ptrans = (bTrans_t *)sockfd;
    return ptrans->readable;
}

uint8_t bSockIsWriteable(int sockfd)
{
    if (sockfd < 0)
    {
        return 0;
    }
    bTrans_t *ptrans = (bTrans_t *)sockfd;
    return ptrans->writeable;
}

int bShutdown(int sockfd)
{
    if (sockfd < 0)
    {
        return -1;
    }
    bTrans_t *ptrans = (bTrans_t *)sockfd;
    __list_del(ptrans->list.prev, ptrans->list.next);
    close(ptrans->sockfd);
    free(ptrans->rx_fifo.pbuf);
    bFIFO_Deinit(&ptrans->rx_fifo);
    free(ptrans);
    ptrans = NULL;
    return 0;
}

int bDnsParse(char *remote, pbTransDnsCb_t cb, void *user_data)
{
    if (remote == NULL || cb == NULL)
    {
        return -1;
    }
    struct hostent  *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(remote)) == NULL)
    {
        printf("gethostbyname fail\r\n");
        return -1;
    }
    addr_list = (struct in_addr **)he->h_addr_list;
    cb(remote, addr_list[0]->s_addr, user_data);
    return 0;
}
int bPing(char *remote, uint32_t timeout_ms, pbTransPingCb_t cb, void *user_data)
{
    return -1;
}

#endif

#endif
