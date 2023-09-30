#ifndef H_GAURD_SVC_SERVER_RESPONSE
#define H_GAURD_SVC_SERVER_RESPONSE

//----------- naming messages.
// M-> Message.

// C-> message to client
// B-> message to channel
// A-> message to all

// F-> Failed
// S-> Success
#define MAS_JOINT_CHANNEL "client joint to the channel succsesfully.\n"
#define MAS_LEFT_CHANNEL "success to leave client from channel."
#define MCF_INVAILD_IDENTITY "client has sent an invalid identity. rejected.\n"
#define MAS_CLIENT_CONNECTED "client has been conencted to the server.\n"
#define MCF_INVAILD_REQUEST_SYNTAX "A: invaild syntax because of deleimited cant..\n"
#define MAS_CLIENT_DISCONNECTED "client disconnnecetd succsesfully.\n"
#define MCF_CLIENT_ISNT_DISCONNECT "failed to disconnect client.\n"
#define MCF_CHANNEL_PASSWORD_INCORRECT "incorrect password to join channel.\n"
#define MCF_YOU_ARE_NOT_ADMIN "client is not admin to create channel.\n"
#define MAS_CHANNEL_CREATED "channel created successfully.\n"
#define MAS_CHANNEL_DELETED "channel has been delted.\n"
#define MAS_CHANNEL_UPDATED "channel has been updated.\n"
#define MAS_CLIENT_RENAMED "client has been change his name\n"
#define MAS_CLIENT_SPEAKER_MUTED ""
#define MAS_CLIENT_SPEAKER_UNMUTED ""
#define MAS_CLIENT_MICROPHONE_MUTED ""
#define MAS_CLIENT_MICROPHONE_UNMUTED ""
#define MCF_UNKNOWN_REQUEST_CODE "unknwon requset code\n"

// #define





//------------ naming charecter response CODE
//R->response C->code
#define RC_CONNECTED 'A'
// #define RC_RECONNECT 'B'
#define RC_DISCONNECTED 'C'
#define RC_CONNECTION_REFEUSED 'D'
#define RC_CLIENT_BAN 'U'
#define RC_YOU_ARE_BANNED 'E'

#define RC_INVAILD_REQUEST_CODE_OR_SYNTAX 'F'

#define RC_CHANNEL_DETAILS 'G'
#define RC_TEXT_MESSAGE 'H'
#define RC_INVALID_CHANNEL_ID 'K'
#define RC_JOINT_TO_CHANNEL 'L'
#define RC_LEFT_FROM_CHANNEL 'M'
#define RC_INCORRECT_CHANNEL_PASSWORD 'N'
#define RC_NO_ONES_READ_YOUR_CHAT_IN_CHANNEL 'O'
#define RC_YOU_ARE_IN_DEFAULT_CHANNEL 'P'

#define RC_CHANNEL_UPDATED 'V'
#define RC_CHANNEL_CREATED 'S'
#define RC_CHANNEL_DELETED 'T'
#define RC_YOU_DONT_HAVE_PERMISSION 'W'


#define RC_CLIENT_RENAMED 'X'
#define RC_CLIENT_SPEAKER_MUTED 'Y'
#define RC_CLIENT_SPEAKER_UNMUTED 'Z'
#define RC_CLIENT_MICROPHONE_MUTED 'Q'
#define RC_CLIENT_MICROPHONE_UNMUTED 'R'


#endif
