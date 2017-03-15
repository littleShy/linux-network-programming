## Chart Room Function Design In Detail

[TOC]

### 概要设计

#### 聊天室功能：
- 注册/登录/退出
- 查看服务端上在线用户
- 建群/邀请加入群
- 私聊/查看聊天记录
- 群聊/查看聊天记录


#### 客户端：
- 登录页面	
	- 注册
	- 登录
- 好友面板
	- 建群
	- 在线好友
	- 退出
- 聊天面板
	* 私聊
	* 群聊/群成员
	* 聊天记录查看
#### 服务端：
- 通讯协议
	- 注册
	- 登录
	- 建群
	- 聊天
	- 记录查询
- 数据存储
	- 用户信息
	- 在线/离线
	- 聊天记录

### 详细设计

#### 类

* DbManager.cpp
```c++
	//处理数据库相关操作
	int StartUpDbManager();
	int GetSqlStatement(const int iSqlType, char *pSql, const int iMaxSqlLen);	//获取SQL
	int ExecQuerySql(const char *pSql);
	int ExecDMLSql(const char *pSql);
```

* NetWorkManager.cpp
```c++
	//处理网络相关操作
	public:
		int StartUpServer();
		int ShutDownServer();
	private:
		int SendPacket(char *packetBuff, const ssize_t buffLen);
		int RecvPacket(char *packetBuff, ssize_t inOutLen);
```

* JsonParser.cpp
```c++
	//处理数据包转换操作
	int InitJsonParser();
	int ParseRecvPacket(char *packetBuff, const ssize_t buffLen, CPackBase *pBase);
	int ParseSendPacket(CPackBase *pBase, char *packetBuff, ssize_t &inOutLen);
```

* UserStatusManager.cpp
```c++
	//用户状态管理,注册、登录、退出
	int InitUserStatusManager();
	bool Login(const CUser user);		//用户登录
	bool Logout(const int userId, const int iLogoutStatus=0);	//用户登出，0手动退出，1其他原因退出
	bool Register(const CUser user);	//用户注册
```

* HeartBeat.cpp
```c++
	bool KeepAlive(const int userId);	//用户10S一次心跳，无心跳时自动下线
```
* GroupManager.cpp
```c++
	bool CreateGroup(const CGroup &group);	//创建群
	int JoinGroup(const int userId, const CGroup &group);
```
* MessageLogManager.cpp
```c++
	int NewMessage();
```

#### 数据库

* 用户表
```sql
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
```
* 群组表
```SQL
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
```
* 聊天记录表
```sql
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
```
#### Json报文
* 注册
```json
{
	"MESSAGE_TYPE": "REGISTER",
	"VERSION": 1.01,
	"USER": {
		"USER_ID": 123456,
		"EMAIL": "",
		"PASSWORD": ""
	}
}
```
```json
{
	"MESSAGE_TYPE": "RESPONSE",
	"VERSION": 1.01,
	"RESPONSE": {
		"RESPONSE_FOR": 0,
		"RETURN_CODE": 0,
		"RETURN_MSG": ""
	},
	"USER": {
		"USER_ID": 123456,
		"EMAIL": "",
		"PASSWORD": ""
	}
}
```
* 登录
```json
{
	"MESSAGE_TYPE": "LOGIN",
	"VERSION": 1.01,
	"LOGIN": {
		"USER_ID": 123456,
		"PASSWORD": "123456"
	}
}
```

```json
{
	"MESSAGE_TYPE": "RESPONSE",
	"VERSION": 1.01,
	"RESPONSE": {
		"RESPONSE_FOR": 1,
		"RETURN_CODE": 0,
		"RETURN_MSG": ""
	}
}
```
* 退出
```json
{
	"MESSAGE_TYPE": "LOGOUT",
	"VERSION": 1.01,
	"USER_ID": ""
}
```
```json
{
	"MESSAGE_TYPE": "RESPONSE",
	"VERSION": 1.01,
	"RESPONSE": {
		"RESPONSE_FOR": 3,
		"RETURN_CODE": 0,
		"RETURN_MSG": ""
	}
}
```
* 私聊
```json
{
	"MESSAGE_TYPE": "CHART_SINGLE",
	"VERSION": 1.01,
	"CHART_SINGLE": {
		"USER_ID": 123456,
		"OPP_USER_ID": 654321,
		"MESSAGE": "Hello.",
	"DATETIME": "20170315131703"
	}
}
```

```json
{
	"MESSAGE_TYPE": "RESPONSE",
	"VERSION": 1.01,
	"RESPONSE": {
		"RESPONSE_FOR": 4,
		"RETURN_CODE": 0,
		"RETURN_MSG": ""
	}
}
```
* 建群
```json
{
	"MESSAGE_TYPE": "CREATE_GROUP",
	"VERSION": 1.01,
	"GROUP": {
		"GROUP_ID": 67890,
		"USER_ID": 123456,
		"MEMBER_LIST_SIZE": 2,
		"MEMBER_ID_LIST": [
		123456,
			23456,
			34567
		]
	}
}
```

```json
{
	"MESSAGE_TYPE": "RESPONSE",
	"VERSION": 1.01,
	"RESPONSE": {
		"RESPONSE_FOR": 5,
		"RETURN_CODE": 0,
		"RETURN_MSG": ""
	}
}
```
* 邀请入群
```json
{
	"MESSAGE_TYPE": "INVITE_JOIN_GROUP",
	"VERSION": 1.01,
	"INVITE_JOIN_GROUP": {
		"INVITE_USER_ID": 123456,
		"INVITED_USER_ID": 234567,
		"GROUP_ID": 67890
	}
}
```
```json
{
	"MESSAGE_TYPE": "RESPONSE",
	"VERSION": 1.01,
	"RESPONSE": {
		"RESPONSE_FOR": 7,
		"RETURN_CODE": 0,
		"RETURN_MSG": ""
	},
	"GROUP": {
		"GROUP_ID": 67890,
		"USER_ID": 123456,
		"MEMBER_LIST_SIZE": 2,
		"MEMBER_ID_LIST": [
		123456,
			23456,
			34567
		]
	}
}
```
* 群聊
```json
{
	"MESSAGE_TYPE": "CHART_GROUP",
	"VERSION": 1.01,
	"GROUP_CHART": {
		"GROUP_ID": 67890,
		"USER_ID": 123456,
		"MESSAGE": "Hello everyone.",
	"DATETIME": "20170315131630"
	}
}
```

```json
{
	"MESSAGE_TYPE": "RESPONSE",
	"VERSION": 1.01,
	"RESPONSE": {
		"RESPONSE_FOR": 8,
		"RETURN_CODE": 0,
		"RETURN_MSG": ""
	}
}
```
* 查询在线好友
```json
{
	"MESSAGE_TYPE": "ONLINE_FRIENDS",
	"VERSION": 1.01,
	"USER_ID": 123456
}
```

```json
{
	"MESSAGE_TYPE": "RESPONSE",
	"VERSION": 1.01,
	"RESPONSE": {
		"RESPONSE_FOR": 9,
		"RETURN_CODE": 0,
		"RETURN_MSG": ""
	},
	"FRIENDS": {
		"USER_ID": 123456,
		"FRIEND_LIST_SIZE": 2,
		"FRIEND_LIST":[
			23456,
			34567
		]
	}
}
```
* 聊天记录查询
```json
{
	"MESSAGE_TYPE": "CHART_LOG_REQUEST",
	"VERSION": 1.01,
	"CHART_LOG_REQUEST": {
		"USER_ID": 123456,
		"OPP_OBJECT_ID": 234567,
		"OPP_OBJECT_TYPE": 0,
		"DATE_BEGIN": "",
		"DATE_END": ""
	}
}
```

```json
		{
			"MESSAGE_TYPE": "RESPONSE",
			"VERSION": 1.01,
			"RESPONSE": {
				"RESPONSE_FOR": 10,
				"RETURN_CODE": 0,
				"RETURN_MSG": ""
			},
			"CHART_LOG": {
				"USER_ID": 123456,
				"OPP_OBJECT_ID": 234567,
				"OPP_OBJECT_TYPE": 0,
				"DATE_BEGIN": "20170301",
				"DATE_END": "20170331",
				"CHART_LOG_LIST_SIZE": 2,
				"CHART_LOG_LIST": [
					{
						"USER_ID":123456,
						"MESSAGE":"Hi.",
						"DATETIME":"20170315123030"
					},
					{
						"USER_ID":23456,
						"MESSAGE":"Hello.",
						"DATETIME":"20170315123040"
					}
				]
			}
		}
```
* 心跳
```json
{
	"MESSAGE_TYPE": "HEARTBEAT",
	"VERSION": 1.01,
	"USER_ID": 123456
}
```

```json
{
	"MESSAGE_TYPE": "RESPONSE",
	"VERSION": 1.01,
	"RESPONSE": {
		"RESPONSE_FOR": 11,
		"RETURN_CODE": 0,
		"RETURN_MSG": ""
	}
}
```

