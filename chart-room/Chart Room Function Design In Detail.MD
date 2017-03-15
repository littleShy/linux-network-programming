Chart Room Function Design In Detail

聊天室功能：
		注册/登录/退出
		查看服务端上在线用户
		建群/邀请加入群
		私聊/查看聊天记录
		群聊/查看聊天记录


客户端：
	1. 
		注册
		登录
		退出
	2. 
		建群
		好友列表
			在线好友
		聊天
			私聊
			群聊
			聊天记录查看
服务端：
	1. 
		通讯协议
			注册/登录/建群/聊天/记录查询
	2. 
		数据存储
			用户信息
			在线、离线
			聊天记录
	3. 


DbManager.cpp		//处理数据库相关操作
	提供对外接口：
	int StartUpDbManager();
	int GetSqlStatement(const int iSqlType, char *pSql, const int iMaxSqlLen);	//获取SQL
	int ExecQuerySql(const char *pSql);
	int ExecDMLSql(const char *pSql);

NetWorkManager.cpp		//处理网络相关操作
	提供对外接口：
	public:
		int StartUpServer();
		int ShutDownServer();
	private:
		int SendPacket(char *packetBuff, const ssize_t buffLen);
		int RecvPacket(char *packetBuff, ssize_t inOutLen);

JsonParser.cpp		//处理数据包转换操作
	提供对外接口：
	int InitJsonParser();
	int ParseRecvPacket(char *packetBuff, const ssize_t buffLen, CPackBase *pBase);
	int ParseSendPacket(CPackBase *pBase, char *packetBuff, ssize_t &inOutLen);

UserStatusManager.cpp	//用户状态管理,注册、登录、退出
	int InitUserStatusManager();
	bool Login(const CUser user);		//用户登录
	bool Logout(const int userId, const int iLogoutStatus=0);	//用户登出，0手动退出，1其他原因退出
	bool Register(const CUser user);	//用户注册

HeartBeat.cpp
	bool KeepAlive(const int userId);	//用户10S一次心跳，无心跳时自动下线
GroupManager.cpp
	bool CreateGroup(const CGroup &group);	//创建群
	int JoinGroup(const int userId, const CGroup &group);
MessageLogManager.cpp
	int NewMessage();

//用户表
CREATE TABLE USER
{
	USER_ID NUMBER;
	PASSWORD CHAR[32];
	EMAIL CHAR[32];
};

//查询在线用户列表
SELECT * FROM USER_STATUS WHERE USER_ID ~= SPECIFIED_ID AND ONLINE = TRUE;
//用户状态表
CREATE TABLE USER_STATUS
{
	USER_ID NUMBER;
	ONLINE BOOL;
};

//群组表
CREATE TABLE GROUP
{
	GROUP_ID NUMBER;		//群ID
	GROUP_OWNER_ID NUMBER;	//群主ID
	GROUP_CREATE_TIME DATE;	//建群日期
};

//群成员表
CREATE TABLE GROUP_MEMBER
{
	GROUP_ID NUMBER;		//群ID
	USER_ID NUMBER;		//群成员ID
	JOIN_TIME DATE;		//入群日期
};


//查询聊天记录时可关联自己
//1. 查询用户聊天记录
(SELECT * FROM CHART_LOG A, CHART_LOG B WHERE A.OPP_USER_ID = B.USER_ID AND A.OPP_TYPE = 0 AND MESSAGE_DATE BETWEEN (BEGIN_DATE, END_DATE)) ORDER BY DATE;

//2. 查询群记录
SELECT * FROM CHART_LOG A WHERE A.OPP_OBJECT_ID = SPECIFIED_ID AND OPP_TYPE = 1 ORDER BY DATE;
CREATE TABLE CHART_LOG
{
	USER_ID NUMBER;
	OPP_OBJECT_ID NUMBER;
	OPP_TYPE NUMBER;	//0 user, 1 group
	MESSAGE_DATE DATE;
	MESSAGE CHAR[512];
};


{
    "MESSAGE_TYPE": "REGISTER",
    "VERSION": 1.01,
    "REGISTER": {
        "USER_ID": 123456,
        "EMAIL": "",
        "PASSWORD": ""
    },
    "LOGIN": {
        "USER_ID": 123456,
        "PASSWORD": ""
    },
    "LOGOUT": {
        "USER_ID": ""
    },
    "CHART_SINGLE": {
        "OPP_USER_ID": 654321,
        "MESSAGE": ""
    },
    "CHART_GROUP": {
        "GROUP_ID": "",
        "USER_ID": "",
        "MESSAGE": ""
    },
    "HEART_BEAT": {
        "USER_ID": 123456
    },
    "CREATE_GROUP_REQUEST": {
        "USER_ID": "",
        "MEMBER_LIST_SIZE": 2,
        "MEMBER_ID_LIST": [
            23456,
            34567
        ]
    },
    "COMMON_RESPONSE_PACKET": {
        "RESPONSE_FOR": 0,
        "RETURN_CODE": 0,
        "RETURN_MSG": ""
    },
    "ONLINE_USER_QUERY_REQUEST": {
        "USER_ID": 123456
    },
    "RECORD_QUERY_REQUEST": {
        "OBJECT_ID": "",
        "OBJECT_TYPE": 0,
        "OPP_OBJECT_ID": 654321,
        "OPP_OBJECT_TYPE": 0,
        "DATE_BEGIN": "",
        "DATE_END": ""
    },
    "RECORD_QUERY_RESPONSE": {
        "OBJECT_ID": "",
        "OBJECT_TYPE": 0,
        "DATE_BEGIN": "",
        "DATE_END": "",
            "MESSAGE_LOG_LIST_SIZE": 2,
            "MESSAGE_LOG_LIST": [
                {
                	USER_ID:123456,
                	MESSAGE:"HI."
                },
                {
                	USER_ID:23456,
                	MESSAGE:"HELLO."
                }
            ]
    }
}