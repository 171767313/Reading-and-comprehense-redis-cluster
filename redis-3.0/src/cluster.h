#ifndef __REDIS_CLUSTER_H
#define __REDIS_CLUSTER_H

/*-----------------------------------------------------------------------------
 * Redis cluster data structures, defines, exported API.
 *----------------------------------------------------------------------------*/

// ������
#define REDIS_CLUSTER_SLOTS 16384  //��Ӧ16K��Ҳ����2��14�η�
// ��Ⱥ����
#define REDIS_CLUSTER_OK 0          /* Everything looks ok */
// ��Ⱥ����
#define REDIS_CLUSTER_FAIL 1        /* The cluster can't work */
// �ڵ����ֵĳ���
#define REDIS_CLUSTER_NAMELEN 40    /* sha1 hex length */
// ��Ⱥ��ʵ�ʶ˿ں� = �û�ָ���Ķ˿ں� + REDIS_CLUSTER_PORT_INCR
#define REDIS_CLUSTER_PORT_INCR 10000 /* Cluster port = baseport + PORT_INCR */

/* The following defines are amunt of time, sometimes expressed as
 * multiplicators of the node timeout value (when ending with MULT). 
 *
 * �����Ǻ�ʱ���йص�һЩ������
 * �� _MULTI ��β�ĳ�������Ϊʱ��ֵ�ĳ˷�������ʹ�á�
 */
// Ĭ�Ͻڵ㳬ʱʱ��
#define REDIS_CLUSTER_DEFAULT_NODE_TIMEOUT 15000
// �������߱���ĳ˷�����
#define REDIS_CLUSTER_FAIL_REPORT_VALIDITY_MULT 2 /* Fail report validity. */
// �������ڵ� FAIL ״̬�ĳ˷�����
#define REDIS_CLUSTER_FAIL_UNDO_TIME_MULT 2 /* Undo fail if master is back. */
// �������ڵ� FAIL ״̬�ļӷ�����
#define REDIS_CLUSTER_FAIL_UNDO_TIME_ADD 10 /* Some additional time. */
// �ڼ��ӽڵ������Ƿ���Чʱʹ�õĳ˷�����
#define REDIS_CLUSTER_SLAVE_VALIDITY_MULT 10 /* Slave data validity. */
// ��ִ�й���ת��֮ǰ��Ҫ�ȴ����������ƺ��Ѿ�����
#define REDIS_CLUSTER_FAILOVER_DELAY 5 /* Seconds */
// δʹ�ã��ƺ��Ѿ�����
#define REDIS_CLUSTER_DEFAULT_MIGRATION_BARRIER 1
// �ڽ����ֶ��Ĺ���ת��֮ǰ����Ҫ�ȴ��ĳ�ʱʱ��
#define REDIS_CLUSTER_MF_TIMEOUT 5000 /* Milliseconds to do a manual failover. */
// δʹ�ã��ƺ��Ѿ�����
#define REDIS_CLUSTER_MF_PAUSE_MULT 2 /* Master pause manual failover mult. */

/* Redirection errors returned by getNodeByQuery(). */
/* �� getNodeByQuery() �������ص�ת����� */
// �ڵ���Դ����������
#define REDIS_CLUSTER_REDIR_NONE 0          /* Node can serve the request. */
// ����������
#define REDIS_CLUSTER_REDIR_CROSS_SLOT 1    /* Keys in different slots. */
// �������Ĳ����ڽ��� reshard
#define REDIS_CLUSTER_REDIR_UNSTABLE 2      /* Keys in slot resharding. */
// ��Ҫ���� ASK ת��
#define REDIS_CLUSTER_REDIR_ASK 3           /* -ASK redirection required. */
// ��Ҫ���� MOVED ת��
#define REDIS_CLUSTER_REDIR_MOVED 4         /* -MOVED redirection required. */

// ǰ�ö��壬��ֹ�������
struct clusterNode;


//�ͻ��������˷���meet�󣬿ͻ���ͨ���ͷ���˽�����������¼����˽ڵ�clusterNode->link��clusterCron
//����˽��յ����Ӻ�ͨ��clusterAcceptHandler�����ͻ��˽ڵ��clusterNode.link����clusterAcceptHandler


//server.cluster(clusterState)->clusterState.nodes(clusterNode)->clusterNode.link(clusterLink)
//redisClient�ṹ��clusterLink�ṹ�����Լ����׽��������������� ������������������ڣ�redisClient���ڿͻ���
//clusterLink���ڼ�Ⱥ�е����ӽڵ�
/* clusterLink encapsulates everything needed to talk with a remote node. */
// clusterLink �������������ڵ����ͨѶ�����ȫ����Ϣ
typedef struct clusterLink { //clusterNode->link     ��Ⱥ���ݽ������յĵط���clusterProcessPacket      
//clusterLink�����ĵط���clusterAcceptHandler->createClusterLink
    //B�ڵ����ӵ�A�ڵ㣬��A�ڵ�ᴴ��һ��clusterLink�����������B�ڵ���ص�����ʱ�䣬���е�node����B�ڵ��clusterNode��fdΪB����A��ʱ���fd

    // ���ӵĴ���ʱ��
    mstime_t ctime;             /* Link creation time */

    // TCP �׽���������
    int fd;                     /* TCP socket file descriptor */

    // ����������������ŵȴ����͸������ڵ����Ϣ��message����
    sds sndbuf;                 /* Packet send buffer */

    // ���뻺�����������Ŵ������ڵ���յ�����Ϣ����clusterReadHandler
    sds rcvbuf;                 /* Packet reception buffer */
 
    // ���������������Ľڵ㣬���û�еĻ���Ϊ NULL   
    //B�ڵ����ӵ�A�ڵ㣬��A�ڵ�ᴴ��һ��clusterLink�����������B�ڵ���ص�����ʱ�䣬���е�node����B�ڵ�
    struct clusterNode *node;   /* Node related to this link if any, or NULL */

} clusterLink;

/*  һ�������Ǹ�ֵ��clusterNode.flag */

/* Cluster node flags and macros. */
// �ýڵ�Ϊ���ڵ�  �ڼ�Ⱥ����£���redis������ʱ���������������clusterģʽ������ñ��ڵ�ģʽΪnodes.conf�е����ã�������������������˺󣬱���
//�����ı��ڵ㱻��Ⱥ�еİ����������ڵ�ѡΪ���ڵ㣬��ýڵ��Ϊ��
#define REDIS_NODE_MASTER 1     /* The node is a master */
// �ýڵ�Ϊ�ӽڵ�
#define REDIS_NODE_SLAVE 2      /* The node is a slave */
// �ýڵ��������ߣ���Ҫ������״̬����ȷ��
#define REDIS_NODE_PFAIL 4      /* Failure? Need acknowledge */
// �ýڵ�������
#define REDIS_NODE_FAIL 8       /* The node is believed to be malfunctioning */
// �ýڵ��ǵ�ǰ�ڵ�����
#define REDIS_NODE_MYSELF 16    /* This node is myself */

//����cluster meet IP port��ʱ����clusterStartHandshake�аѽڵ�״̬��ΪREDIS_NODE_HANDSHAKE  REDIS_NODE_MEET �����ߴ������ļ�node.conf�ж����ľ��Ǹ�״̬
// �ýڵ㻹δ�뵱ǰ�ڵ���ɵ�һ�� PING - PONG ͨѶ   ֻ�н��ܵ�ĳ��node��ping pong meet��������״̬
#define REDIS_NODE_HANDSHAKE 32 /* We have still to exchange the first ping */
// �ýڵ�û�е�ַ  clusterProcessPacketֵ��Ϊ��״̬�����ߴ������ļ�node.conf�ж����ľ��Ǹ�״̬
#define REDIS_NODE_NOADDR   64  /* We don't know the address of this node */

//����cluster meet IP port��ʱ����clusterStartHandshake�аѽڵ�״̬��ΪREDIS_NODE_HANDSHAKE  REDIS_NODE_MEET�����ߴ������ļ�node.conf�ж����ľ��Ǹ�״̬

// ��ǰ�ڵ㻹δ��ýڵ���й��Ӵ�
// ���������ʶ���õ�ǰ�ڵ㷢�� MEET ��������� PING ����
#define REDIS_NODE_MEET 128     /* Send a MEET message to this node */
// �ýڵ㱻ѡ��Ϊ�µ����ڵ�
#define REDIS_NODE_PROMOTED 256 /* Master was a slave propoted by failover */
// �����֣��ڽڵ�Ϊ���ڵ�ʱ��������Ϣ�е� slaveof ���Ե�ֵ��
#define REDIS_NODE_NULL_NAME "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"

// �����жϽڵ���ݺ�״̬��һϵ�к�
#define nodeIsMaster(n) ((n)->flags & REDIS_NODE_MASTER)
#define nodeIsSlave(n) ((n)->flags & REDIS_NODE_SLAVE)
#define nodeInHandshake(n) ((n)->flags & REDIS_NODE_HANDSHAKE)
#define nodeHasAddr(n) (!((n)->flags & REDIS_NODE_NOADDR))
#define nodeWithoutAddr(n) ((n)->flags & REDIS_NODE_NOADDR)
#define nodeTimedOut(n) ((n)->flags & REDIS_NODE_PFAIL)
#define nodeFailed(n) ((n)->flags & REDIS_NODE_FAIL)

/* This structure represent elements of node->fail_reports. */
// ÿ�� clusterNodeFailReport �ṹ������һ�������ڵ��Ŀ��ڵ�����߱���
// ����ΪĿ��ڵ��Ѿ����ߣ�
struct clusterNodeFailReport {

    // ����Ŀ��ڵ��Ѿ����ߵĽڵ�
    struct clusterNode *node;  /* Node reporting the failure condition. */

    // ���һ�δ� node �ڵ��յ����߱����ʱ��
    // ����ʹ�����ʱ�����������߱����Ƿ����
    mstime_t time;             /* Time of the last report from this node. */

} typedef clusterNodeFailReport;

//server.cluster(clusterState)->clusterState.nodes(clusterNode)->clusterNode.link(clusterLink)
// �ڵ�״̬    �ڵ㴴����createClusterNode
struct clusterNode { //clusterState->nodes�ṹ  ��Ⱥ���ݽ������յĵط���clusterProcessPacket

    // �����ڵ��ʱ��
    mstime_t ctime; /* Node object creation time. */

    // �ڵ�����֣��� 40 ��ʮ�������ַ����   ��createClusterNode->getRandomHexChars
    // ���� 68eef66df23420a5862208ef5b1a7005b806f2ff
    char name[REDIS_CLUSTER_NAMELEN]; /* Node name, hex string, sha1-size */

    // �ڵ��ʶ
    // ʹ�ø��ֲ�ͬ�ı�ʶֵ��¼�ڵ�Ľ�ɫ���������ڵ���ߴӽڵ㣩��
    // �Լ��ڵ�Ŀǰ������״̬���������߻������ߣ���  ȡֵREDIS_NODE_MASTER  REDIS_NODE_PFAIL��
    int flags;      /* REDIS_NODE_... */ //ȡֵ���Բο�clusterGenNodeDescription

    /* configepoch��currentepoch���Բο�:Redis_Cluster��Failover���.PPT */
    // �ڵ㵱ǰ�����ü�Ԫ������ʵ�ֹ���ת�� /* current epoch��cluster epoch���Բο�http://redis.cn/topics/cluster-spec.html */
    //����ͨ��cluster set-config-epoch num������configEpoch,��clusterCommand
    //ʵ���ϸ����ڵ��Լ���configEpochͨ�����Ľ�������clusterHandleConfigEpochCollision�н������ã�
    //Ҳ��������server.cluster->currentEpoch��ͼ�Ⱥ�нڵ�node.configEpochֵ������ͬ�����û������set-config-epoch�Ļ���
    //Ҳ���Ǽ�Ⱥ�ڵ�����ȥ1(��Ϊepoch��0��ʼ)

    /*
    Current Epoch���ڼ�Ⱥ��epoch������Ⱥ�İ汾��
    Config Epoch��ÿ��master����config Epoch����Master�İ汾
    ÿ���¼���Ľڵ㣬current Epoch��ʼΪ0��ͨ��ping/pong��Ϣ�����Ļ���������ͽڵ��epoch�����Լ��ģ�
    ���Լ���currentEpoch����Ϊ�����ߵġ�����N����Ϣ�����Ժ�ÿ���ڵ��current epoch����һ�¡�
    clusterState.currentEpoch����ͨ��cluster info�����е�cluster_current_epoch��ȡ��
    Current Epoch����failover

    
    Redis�ṩ�˽����ͻ�İ취���ڵ�֮����Ϣ���������У�����Լ���currentEpoch��configEpoch����ȥ��������ַ�����
    ��configEpoch���Լ���configEpoch��ͬ�����Լ���Epoch+1������N���Ժ�ʹÿ��master��configEpoch��һ��
    SlaveҲ��configEpoch����ͨ��master�����õ���Master��configEpoch����currentEpoch��������Ⱥ�İ汾�ţ����нڵ��ֵ��ͬ
    
    ÿ��master��configEpoch���벻ͬ�����������ó�ͻ�Ժ󣬲��ø߰汾�����á�ͨ����ν����󣬼�Ⱥ��ÿ���ڵ��configEpoch
    �᲻ͬ�����û������set-config-epoch�Ļ��������ڵ��clusterNode.configEpoch�ֱ�Ϊ0 - n,����3���ڵ㣬��ÿ���ڵ�ֱ��Ӧ
    0 1 2������ͨ��cluster node��connectedǰ����ֵ�鿴
    */ //��ֵ��clusterHandleConfigEpochCollision
    uint64_t configEpoch; /* Last configEpoch observed for this node */

    // ������ڵ㸺����Ĳ�
    // һ���� REDIS_CLUSTER_SLOTS / 8 ���ֽڳ�
    // ÿ���ֽڵ�ÿ��λ��¼��һ���۵ı���״̬
    // λ��ֵΪ 1 ��ʾ�����ɱ��ڵ㴦��ֵΪ 0 ���ʾ�۲��Ǳ��ڵ㴦��
    // ���� slots[0] �ĵ�һ��λ�����˲� 0 �ı������
    // slots[0] �ĵڶ���λ�����˲� 1 �ı���������Դ�����     λͼ��ʾ16384����λ  ��¼��clusterNode�ڵ㴦��Ĳ�
    
    //clusterNode->slots��¼��clusterNode�ڵ㴦��Ĳۣ�clusterState->nodes��¼�����е�clusterNode�ڵ���Ϣ�������ڵ��slots
    //ָ���˱��ڵ㴦��Ĳۣ����clusterState->nodes���Ի�ȡ�����в��������Ǹ��ڵ�
    unsigned char slots[REDIS_CLUSTER_SLOTS/8]; /* slots handled by this node */

    // �ýڵ㸺����Ĳ�����
    int numslots;   /* Number of slots handled by this node */

    // ������ڵ������ڵ㣬��ô��������Լ�¼�ӽڵ������
    int numslaves;  /* Number of slave nodes, if this is a master */

    // ָ�����飬ָ������ӽڵ�
    struct clusterNode **slaves; /* pointers to slave nodes */

    // �������һ���ӽڵ㣬��ôָ�����ڵ�
    struct clusterNode *slaveof; /* pointer to the master node */

    //���node�ڵ����һ�η���ping��Ϣ��ʱ��       ����ping�Զˣ��Զ�pongӦ����Ѹ�ping_pong��0����clusterProcessPacket
    // ���һ�η��� PING �����ʱ��   ��ֵ��clusterSendPing
    mstime_t ping_sent;      /* Unix time we sent latest ping */

    // ���һ�ν��� PONG �ظ���ʱ���
    mstime_t pong_received;  /* Unix time we received the pong */

    // ���һ�α�����Ϊ FAIL ״̬��ʱ��
    mstime_t fail_time;      /* Unix time when FAIL flag was set */

    // ���һ�θ�ĳ���ӽڵ�ͶƱ��ʱ��
    mstime_t voted_time;     /* Last time we voted for a slave of this master */

    // ���һ�δ�����ڵ���յ�����ƫ������ʱ��
    mstime_t repl_offset_time;  /* Unix time we received offset for this node */

    // ����ڵ�ĸ���ƫ����
    long long repl_offset;      /* Last known repl offset for this node. */

    // �ڵ�� IP ��ַ
    char ip[REDIS_IP_STR_LEN];  /* Latest known IP address of this node */

    // �ڵ�Ķ˿ں�
    int port;                   /* Latest known port of this node */

    
    //�ͻ��������˷���meet�󣬿ͻ���ͨ���ͷ���˽�����������¼����˽ڵ�clusterNode->link��clusterCron
    //����˽��յ����Ӻ�ͨ��clusterAcceptHandler�����ͻ��˽ڵ��clusterNode.link����clusterAcceptHandler
    
    // �������ӽڵ�������й���Ϣ   link�ڵ㴴���͸�ֵ��clusterCron.createClusterLink
    clusterLink *link;          /* TCP/IP link with this node */

    // һ��������¼�����������ڵ�Ըýڵ�����߱���
    list *fail_reports;         /* List of nodes signaling this as failing */

};
typedef struct clusterNode clusterNode;


// ��Ⱥ״̬��ÿ���ڵ㶼������һ��������״̬����¼���������еļ�Ⱥ�����ӡ�
// ���⣬��Ȼ����ṹ��Ҫ���ڼ�¼��Ⱥ�����ԣ�����Ϊ�˽�Լ��Դ��
// ��Щ��ڵ��йص����ԣ����� slots_to_keys �� failover_auth_count 
// Ҳ���ŵ�������ṹ���档   ��Ⱥ���ݽ������յĵط���clusterProcessPacket

//server.cluster(clusterState)->clusterState.nodes(clusterNode)->clusterNode.link(clusterLink)
typedef struct clusterState { //����Դͷ��server.cluster   //��Ⱥ������ü�����clusterLoadConfig

    // ָ��ǰ�ڵ��ָ��
    clusterNode *myself;  /* This node */

    /* current epoch��cluster epoch���Բο�http://redis.cn/topics/cluster-spec.html */
    // ��Ⱥ��ǰ�����ü�Ԫ������ʵ�ֹ���ת��   ���Ҳ���Ǽ�Ⱥ�����нڵ������currentEpoch 

    //ʵ���ϸ����ڵ��Լ���configEpochͨ�����Ľ�������clusterHandleConfigEpochCollision�н������ã�
    //Ҳ��������server.cluster->currentEpoch��ͼ�Ⱥ�нڵ�node.configEpochֵ������ͬ�����û������set-config-epoch�Ļ���
    //Ҳ���Ǽ�Ⱥ�ڵ�����ȥ1(��Ϊepoch��0��ʼ)
    //Epoch��һ��ֻ���İ汾�š�ÿ�����¼�������epoch����������������¼���ָ�ڵ���롢failover��
     /*
    Current Epoch���ڼ�Ⱥ��epoch������Ⱥ�İ汾��
    Config Epoch��ÿ��master����config Epoch����Master�İ汾
    ÿ���¼���Ľڵ㣬current Epoch��ʼΪ0��ͨ��ping/pong��Ϣ�����Ļ���������ͽڵ��epoch�����Լ��ģ�
    ���Լ���currentEpoch����Ϊ�����ߵġ�����N����Ϣ�����Ժ�ÿ���ڵ��current epoch����һ�¡�
    clusterState.currentEpoch����ͨ��cluster info�����е�cluster_current_epoch��ȡ��
    Current Epoch����failover

    
    Redis�ṩ�˽����ͻ�İ취���ڵ�֮����Ϣ���������У�����Լ���currentEpoch��configEpoch����ȥ��������ַ�����
    ��configEpoch���Լ���configEpoch��ͬ�����Լ���Epoch+1������N���Ժ�ʹÿ��master��configEpoch��һ��
    SlaveҲ��configEpoch����ͨ��master�����õ���Master��configEpoch����currentEpoch��������Ⱥ�İ汾�ţ����нڵ��ֵ��ͬ
    
    ÿ��master��configEpoch���벻ͬ�����������ó�ͻ�Ժ󣬲��ø߰汾�����á�ͨ����ν����󣬼�Ⱥ��ÿ���ڵ��configEpoch
    �᲻ͬ�����û������set-config-epoch�Ļ��������ڵ��clusterNode.configEpoch�ֱ�Ϊ0 - n,����3���ڵ㣬��ÿ���ڵ�ֱ��Ӧ
    0 1 2������ͨ��cluster node��connectedǰ����ֵ�鿴
    */ //currentEpoch����ͨ��cluster info�����е�cluster_current_epoch��ȡ��   configEpoch����ͨ��cluster node��connectedǰ����ֵ�鿴
    uint64_t currentEpoch; //��clusterHandleConfigEpochCollision�л�����

    // ��Ⱥ��ǰ��״̬�������߻�������    ��clusterUpdateState�и��¼�Ⱥ״̬
    int state;            /* REDIS_CLUSTER_OK, REDIS_CLUSTER_FAIL, ... */

    // ��Ⱥ�����ٴ�����һ���۵Ľڵ��������  clusterUpdateState�и���   ���߲������ڴ�������һ���۵� master ������
    //ע��:���������ߵģ���Ϊ�����ߵ�node���ǻ���dict *nodes�� 
    int size;             /* Num of master nodes with at least one slot */ //Ĭ�ϴ�1��ʼ�������Ǵ�0��ʼ

    //�ڵ�Bͨ��cluster meet A-IP A-PORT��B�ڵ���ӵ�A�ڵ��ڼ�Ⱥ��ʱ��A�ڵ��nodes����ͻ���B�ڵ����Ϣ
    //Ȼ��A�ڵ�Ӧ��pong��B��B�յ���Ҳ���A�ڵ���ӵ��Լ���nodes��
    // ��Ⱥ�ڵ����������� myself �ڵ㣩
    // �ֵ�ļ�Ϊ�ڵ�����֣��ֵ��ֵΪ clusterNode �ṹ

    //clusterNode->slots��¼��clusterNode�ڵ㴦��Ĳۣ�clusterState->nodes��¼�����е�clusterNode�ڵ���Ϣ�������ڵ��slots
    //ָ���˱��ڵ㴦��Ĳۣ����clusterState->nodes���Ի�ȡ�����в��������Ǹ��ڵ�

    //ע��:����ӵ���Ⱥ�е�ĳ���ڵ������ˣ�������ڵ��clusterNode���ǻ��ڸ�nodes���棬ֻ��cluster nodes��ʱ���Ѹýڵ���Ϊ����
    dict *nodes;          /* Hash table of name -> clusterNode structures */

    // �ڵ������������ CLUSTER FORGET ����
    // ��ֹ�� FORGET ���������±���ӵ���Ⱥ����
    // �����������ƺ�û����ʹ�õ����ӣ��ѷ�����������δʵ�֣���
    dict *nodes_black_list; /* Nodes we don't re-add for a few seconds. */

    // ��¼Ҫ�ӵ�ǰ�ڵ�Ǩ�Ƶ�Ŀ��ڵ�Ĳۣ��Լ�Ǩ�Ƶ�Ŀ��ڵ�
    // migrating_slots_to[i] = NULL ��ʾ�� i δ��Ǩ��
    // migrating_slots_to[i] = clusterNode_A ��ʾ�� i Ҫ�ӱ��ڵ�Ǩ�����ڵ� A
    clusterNode *migrating_slots_to[REDIS_CLUSTER_SLOTS];

    // ��¼Ҫ��Դ�ڵ�Ǩ�Ƶ����ڵ�Ĳۣ��Լ�����Ǩ�Ƶ�Դ�ڵ�
    // importing_slots_from[i] = NULL ��ʾ�� i δ���е���
    // importing_slots_from[i] = clusterNode_A ��ʾ���ӽڵ� A �е���� i
    clusterNode *importing_slots_from[REDIS_CLUSTER_SLOTS];

    // ����������۵Ľڵ�
    // ���� slots[i] = clusterNode_A ��ʾ�� i �ɽڵ� A ����
    clusterNode *slots[REDIS_CLUSTER_SLOTS];

    // ��Ծ�������Բ���Ϊ��ֵ������Ϊ��Ա���Բ۽�����������
    // ����Ҫ��ĳЩ�۽������䣨range������ʱ�������Ծ������ṩ����
    // ������������� db.c ����

     //ע���ڴ�rdb�ļ�����aof�ļ��ж�ȡ��key-value�Ե�ʱ����������˼�Ⱥ���ܻ���dbAdd->slotToKeyAdd(key);�а�key��slot�Ķ�Ӧ��ϵ��ӵ�slots_to_keys
    //����verifyClusterConfigWithData->clusterAddSlot�дӶ�ָ�ɶ�Ӧ��slot��Ҳ���Ǳ��������е�rdb�е�key-value��Ӧ��slot�������������
    zskiplist *slots_to_keys; 
   

    /* The following fields are used to take the slave state on elections. */
    // ������Щ�����ڽ��й���ת��ѡ��

    // �ϴ�ִ��ѡ�ٻ����´�ִ��ѡ�ٵ�ʱ��
    mstime_t failover_auth_time; /* Time of previous or next election. */

    // �ڵ��õ�ͶƱ����
    int failover_auth_count;    /* Number of votes received so far. */

    // ���ֵΪ 1 ����ʾ���ڵ��Ѿ��������ڵ㷢����ͶƱ����
    int failover_auth_sent;     /* True if we already asked for votes. */

    int failover_auth_rank;     /* This slave rank for current auth request. */

    uint64_t failover_auth_epoch; /* Epoch of the current election. */

    /* Manual failover state in common. */
    /* ���õ��ֶ�����ת��״̬ */

    // �ֶ�����ת��ִ�е�ʱ������    CLUSTER FAILOVER����ᴥ�������ֶ�����ת�ƣ���clusterCommand
    mstime_t mf_end;            /* Manual failover time limit (ms unixtime).
                                   It is zero if there is no MF in progress. */
    /* Manual failover state of master. */
    /* �����������ֶ�����ת��״̬ */
    clusterNode *mf_slave;      /* Slave performing the manual failover. */
    /* Manual failover state of slave. */
    /* �ӷ��������ֶ�����ת��״̬ */
    long long mf_master_offset; /* Master offset the slave needs to start MF
                                   or zero if stil not received. */
    // ָʾ�ֶ�����ת���Ƿ���Կ�ʼ�ı�־ֵ
    // ֵΪ�� 0 ʱ��ʾ���������������Կ�ʼͶƱ
    int mf_can_start;           /* If non-zero signal that the manual failover
                                   can start requesting masters vote. */

    /* The followign fields are uesd by masters to take state on elections. */
    /* ������Щ������������ʹ�ã����ڼ�¼ѡ��ʱ��״̬ */

    // ��Ⱥ���һ�ν���ͶƱ�ļ�Ԫ
    uint64_t lastVoteEpoch;     /* Epoch of the last vote granted. */

    // �ڽ����¸��¼�ѭ��֮ǰҪ�������飬�Ը��� flag ����¼
    int todo_before_sleep; /* Things to do in clusterBeforeSleep(). */

    // ͨ�� cluster ���ӷ��͵���Ϣ����
    long long stats_bus_messages_sent;  /* Num of msg sent via cluster bus. */

    // ͨ�� cluster ���յ�����Ϣ����   �����ڵ㷢�����ڵ�ı����ֽ���
    long long stats_bus_messages_received; /* Num of msg rcvd via cluster bus.*/

} clusterState;

/* clusterState todo_before_sleep flags. */
// ����ÿ�� flag ������һ���������ڿ�ʼ��һ���¼�ѭ��֮ǰ
// Ҫ��������
#define CLUSTER_TODO_HANDLE_FAILOVER (1<<0)
#define CLUSTER_TODO_UPDATE_STATE (1<<1)
#define CLUSTER_TODO_SAVE_CONFIG (1<<2)
#define CLUSTER_TODO_FSYNC_CONFIG (1<<3)

/* Redis cluster messages header */

/* Note that the PING, PONG and MEET messages are actually the same exact
 * kind of packet. PONG is the reply to ping, in the exact format as a PING,
 * while MEET is a special PING that forces the receiver to add the sender
 * as a node (if it is not already in the list). */

/* ������Щ��ֵ��clusterMsg.type   ������Ϣ�Ĵ���ͳһ��clusterReadHandler->clusterProcessPacket */
 
// ע�⣬PING �� PONG �� MEET ʵ������ͬһ����Ϣ��
// PONG �Ƕ� PING �Ļظ�������ʵ�ʸ�ʽҲΪ PING ��Ϣ��
// �� MEET ����һ������� PING ��Ϣ������ǿ����Ϣ�Ľ����߽���Ϣ�ķ�������ӵ���Ⱥ��
// ������ڵ���δ�ڽڵ��б��еĻ���
// PING  MEET��Ϣ��PING��Ϣ����clusterCron�з���
#define CLUSTERMSG_TYPE_PING 0          /* Ping */
// PONG ���ظ� PING��
#define CLUSTERMSG_TYPE_PONG 1          /* Pong (reply to Ping) */
// ����ĳ���ڵ���ӵ���Ⱥ��   MEET��Ϣ��PING��Ϣ����clusterCron�з���
#define CLUSTERMSG_TYPE_MEET 2          /* Meet "let's join" message */
// ��ĳ���ڵ���Ϊ FAIL   ͨ��clusterBuildMessageHdr�������
#define CLUSTERMSG_TYPE_FAIL 3          /* Mark node xxx as failing */
// ͨ�������붩�Ĺ��ܹ㲥��Ϣ
#define CLUSTERMSG_TYPE_PUBLISH 4       /* Pub/Sub Publish propagation */
// ������й���ת�Ʋ�����Ҫ����Ϣ�Ľ�����ͨ��ͶƱ��֧����Ϣ�ķ�����
#define CLUSTERMSG_TYPE_FAILOVER_AUTH_REQUEST 5 /* May I failover? */
// ��Ϣ�Ľ�����ͬ������Ϣ�ķ�����ͶƱ
#define CLUSTERMSG_TYPE_FAILOVER_AUTH_ACK 6     /* Yes, you have my vote */
// �۲����Ѿ������仯����Ϣ������Ҫ����Ϣ�����߽�����Ӧ�ĸ���    ͨ��clusterBuildMessageHdr�������
#define CLUSTERMSG_TYPE_UPDATE 7        /* Another node slots configuration */
// Ϊ�˽����ֶ�����ת�ƣ���ͣ�����ͻ���
#define CLUSTERMSG_TYPE_MFSTART 8       /* Pause clients for manual failover */

/* Initially we don't know our "name", but we'll find it once we connect
 * to the first node, using the getsockname() function. Then we'll use this
 * address for all the next messages. */
//clusterMsg�Ǽ�Ⱥ�ڵ�ͨ�ŵ���Ϣͷ����Ϣ���ǽṹclusterMsgData��
//clusterMsgData����clusterMsgDataGossip��clusterMsgDataFail��clusterMsgDataPublish��clusterMsgDataUpdate
typedef struct {  //ping  pong meet��Ϣ�øýṹ����clusterProcessPacket

    // �ڵ������
    // �ڸտ�ʼ��ʱ�򣬽ڵ�����ֻ��������
    // �� MEET ��Ϣ���Ͳ��õ��ظ�֮�󣬼�Ⱥ�ͻ�Ϊ�ڵ�������ʽ������
    char nodename[REDIS_CLUSTER_NAMELEN];

    // ���һ����ýڵ㷢�� PING ��Ϣ��ʱ���
    uint32_t ping_sent;

    // ���һ�δӸýڵ���յ� PONG ��Ϣ��ʱ���
    uint32_t pong_received;

    // �ڵ�� IP ��ַ
    char ip[REDIS_IP_STR_LEN];    /* IP address last time it was seen */

    // �ڵ�Ķ˿ں�
    uint16_t port;  /* port last time it was seen */

    // �ڵ�ı�ʶֵ
    uint16_t flags;

    // �����ֽڣ���ʹ��
    uint32_t notused; /* for 64 bit alignment */

} clusterMsgDataGossip;

//clusterMsg�Ǽ�Ⱥ�ڵ�ͨ�ŵ���Ϣͷ����Ϣ���ǽṹclusterMsgData��
//clusterMsgData����clusterMsgDataGossip��clusterMsgDataFail��clusterMsgDataPublish��clusterMsgDataUpdate
typedef struct {

    // ���߽ڵ������
    char nodename[REDIS_CLUSTER_NAMELEN];

} clusterMsgDataFail;

//clusterMsg�Ǽ�Ⱥ�ڵ�ͨ�ŵ���Ϣͷ����Ϣ���ǽṹclusterMsgData��
//clusterMsgData����clusterMsgDataGossip��clusterMsgDataFail��clusterMsgDataPublish��clusterMsgDataUpdate
typedef struct {

    // Ƶ��������
    uint32_t channel_len;

    // ��Ϣ����
    uint32_t message_len;

    // ��Ϣ���ݣ���ʽΪ Ƶ����+��Ϣ
    // bulk_data[0:channel_len-1] ΪƵ����
    // bulk_data[channel_len:channel_len+message_len-1] Ϊ��Ϣ
    unsigned char bulk_data[8]; /* defined as 8 just for alignment concerns. */

} clusterMsgDataPublish;

//clusterMsg�Ǽ�Ⱥ�ڵ�ͨ�ŵ���Ϣͷ����Ϣ���ǽṹclusterMsgData��
//clusterMsgData����clusterMsgDataGossip��clusterMsgDataFail��clusterMsgDataPublish��clusterMsgDataUpdate
typedef struct {

    // �ڵ�����ü�Ԫ  /* current epoch��cluster epoch���Բο�http://redis.cn/topics/cluster-spec.html */
    uint64_t configEpoch; /* Config epoch of the specified instance. */

    // �ڵ������
    char nodename[REDIS_CLUSTER_NAMELEN]; /* Name of the slots owner. */

    // �ڵ�Ĳ۲���
    unsigned char slots[REDIS_CLUSTER_SLOTS/8]; /* Slots bitmap. */

} clusterMsgDataUpdate;

//clusterMsg�Ǽ�Ⱥ�ڵ�ͨ�ŵ���Ϣͷ����Ϣ���ǽṹclusterMsgData��
//clusterMsgData����clusterMsgDataGossip��clusterMsgDataFail��clusterMsgDataPublish��clusterMsgDataUpdate
union clusterMsgData {//clusterMsg�е�data�ֶ�

     /* PING, MEET and PONG */ /*
    ��ΪMEET��PING��PONG������Ϣ��ʹ����ͬ����Ϣ���ģ����Խڵ�ͨ����Ϣͷ��type�������ж�һ����Ϣ��MEET��Ϣ��PING��Ϣ����PONG��Ϣ��
     */
    struct {
        /* Array of N clusterMsgDataGossip structures */
        // ÿ����Ϣ���������� clusterMsgDataGossip �ṹ     ?????????Ϊʲô������Դ�������Ա����
        clusterMsgDataGossip gossip[1];  
    } ping;

    /* FAIL */
    struct {
        clusterMsgDataFail about;
    } fail;

    /* PUBLISH */
    struct {
        clusterMsgDataPublish msg;
    } publish;

    /* UPDATE */
    struct {
        clusterMsgDataUpdate nodecfg;
    } update;

};

//clusterMsg�Ǽ�Ⱥ�ڵ�ͨ�ŵ���Ϣͷ����Ϣ���ǽṹclusterMsgData��
//clusterMsgData����clusterMsgDataGossip��clusterMsgDataFail��clusterMsgDataPublish��clusterMsgDataUpdate


//clustermsg�Ǽ�Ⱥ�ڵ�ͨ�ŵ���Ϣͷ����Ϣ���ǽṹclusterMsgData
// ������ʾ��Ⱥ��Ϣ�Ľṹ����Ϣͷ��header��   clusterMsg��clusterBuildMessageHdr�н������
typedef struct { //�ڲ�ͨ��ֱ��ͨ���ýṹ���ͣ������ýṹ��clusterProcessPacket
    char sig[4];        /* Siganture "RCmb" (Redis Cluster message bus). */
    // ��Ϣ�ĳ��ȣ����������Ϣͷ�ĳ��Ⱥ���Ϣ���ĵĳ��ȣ�
    uint32_t totlen;    /* Total length of this message */
    uint16_t ver;       /* Protocol version, currently set to 0. */
    uint16_t notused0;  /* 2 bytes not used. */

    /*
    ��ΪMEET��PING��PONG������Ϣ��ʹ����ͬ����Ϣ���ģ����Խڵ�ͨ����Ϣͷ��type�������ж�һ����Ϣ��MEET��Ϣ��PING��Ϣ����PONG��Ϣ��
     */
    // ��Ϣ������  ȡֵCLUSTERMSG_TYPE_PING��
    uint16_t type;      /* Message type */

    // ��Ϣ���İ����Ľڵ���Ϣ����
    // ֻ�ڷ��� MEET �� PING �� PONG ������ Gossip Э����Ϣʱʹ��
    uint16_t count;     /* Only used for some kind of messages. */ //����Я������Ϣ����������Բο�clusterSendPing

    //��ֵ������server.cluster->currentEpoch����clusterBuildMessageHdr  
    // ��Ϣ�����ߵ����ü�Ԫ   Ҳ���ǵ�ǰ�ڵ����ڼ�Ⱥ�İ汾��  /* current epoch��cluster epoch���Բο�http://redis.cn/topics/cluster-spec.html */
    uint64_t currentEpoch;  /* The epoch accordingly to the sending node. */

    // �����Ϣ��������һ�����ڵ㣬��ô�����¼������Ϣ�����ߵ����ü�Ԫ
    // �����Ϣ��������һ���ӽڵ㣬��ô�����¼������Ϣ���������ڸ��Ƶ����ڵ�����ü�Ԫ 
    /* current epoch��cluster epoch���Բο�http://redis.cn/topics/cluster-spec.html */
    //��clusterBuildMessageHdr����ǰ�ڵ��Epoch��ÿ���ڵ��Լ���Epoch��һ�������Բο�clusterNode->configEpoch
    uint64_t configEpoch;   /* The config epoch if it's a master, or the last
                               epoch advertised by its master if it is a
                               slave. */

    // �ڵ�ĸ���ƫ����
    uint64_t offset;    /* Master replication offset if node is a master or
                           processed replication offset if node is a slave. */

    /* currentEpoch��sender��myslots�����Լ�¼�˷���������Ľڵ���Ϣ�������߻������Щ��Ϣ�����Լ���clusterState��nodes�ֵ����ҵ�����
�߶�Ӧ��clusterNode�ṹ�����Խṹ���и��¡� */
    // ��Ϣ�����ߵ����֣�ID��
    char sender[REDIS_CLUSTER_NAMELEN]; /* Name of the sender node */

    // ��Ϣ������Ŀǰ�Ĳ�ָ����Ϣ
    unsigned char myslots[REDIS_CLUSTER_SLOTS/8];

    // �����Ϣ��������һ���ӽڵ㣬��ô�����¼������Ϣ���������ڸ��Ƶ����ڵ������
    // �����Ϣ��������һ�����ڵ㣬��ô�����¼���� REDIS_NODE_NULL_NAME
    // ��һ�� 40 �ֽڳ���ֵȫΪ 0 ���ֽ����飩
    char slaveof[REDIS_CLUSTER_NAMELEN];

    char notused1[32];  /* 32 bytes reserved for future usage. */

    // ��Ϣ�����ߵĶ˿ں�
    uint16_t port;      /* Sender TCP base port */

    // ��Ϣ�����ߵı�ʶֵ
    uint16_t flags;     /* Sender node flags */

    // ��Ϣ������������Ⱥ��״̬
    unsigned char state; /* Cluster state from the POV of the sender */

    // ��Ϣ��־   ȡֵCLUSTERMSG_FLAG0_PAUSED��
    unsigned char mflags[3]; /* Message flags: CLUSTERMSG_FLAG[012]_... */

    // ��Ϣ�����ģ�����˵�����ݣ�
    union clusterMsgData data;

} clusterMsg;

#define CLUSTERMSG_MIN_LEN (sizeof(clusterMsg)-sizeof(union clusterMsgData))

/* Message flags better specify the packet content or are used to
 * provide some information about the node state. */
#define CLUSTERMSG_FLAG0_PAUSED (1<<0) /* Master paused for manual failover. */
#define CLUSTERMSG_FLAG0_FORCEACK (1<<1) /* Give ACK to AUTH_REQUEST even if
                                            master is up. */

/* ---------------------- API exported outside cluster.c -------------------- */
clusterNode *getNodeByQuery(redisClient *c, struct redisCommand *cmd, robj **argv, int argc, int *hashslot, int *ask);

#endif /* __REDIS_CLUSTER_H */
