#ifdef DECHNICCRYPT_EXPORTS
#define DECHNICCRYPT_API __declspec(dllexport)
#else
#define DECHNICCRYPT_API __declspec(dllimport)
#endif


#pragma comment (lib, "crypt32.lib")
#include <stdio.h>
#include <malloc.h>
#include <windows.h>
#include <wincrypt.h>
#define MY_ENCODING_TYPE (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

//成功
#define DECHNIC_CRYPT_SUCCESS                        0x0
//非法的参数
#define DECHNIC_CRYPT_INVALID_PARA                   0x00002001
//获取证书数据错误
#define DECHNIC_CRYPT_GET_CERT_DATA                  0x00002002
//签名运算错误
#define DECHNIC_CRYPT_SIGN_DATA                      0x00002003
//验签运算错误
#define DECHNIC_CRYPT_VERIFY_DATA                    0x00002004
//签名验证失败
#define DECHNIC_CRYPT_BAD_SIGNATURE                  0x00002005
//加密运算错误
#define DECHNIC_CRYPT_ENCRYPT_DATA                   0x00002006
//解密运算错误
#define DECHNIC_CRYPT_DECRYPT_DATA                   0x00002007
//缓冲区太小
#define DECHNIC_CRYPT_MORE_DATA                      0x00002008
//未找到可用证书
#define DECHNIC_CRYPT_INVALID_CERT                   0x00002009
//内存不足
#define DECHNIC_CRYPT_MALLOC_ERR                     0x0000200A
//证书口令未验证
#define DECHNIC_CRYPT_NO_CHECK_PIN_ERR               0x0000200E



//base64 编码错误
#define S_BASE64ENCODE_ERR						0x00003001
//base64 解码错误
#define S_BASE64DECODE_ERR						0x00003002
//分配空间错误
#define S_MALLOC_ERR							0x00003003
//请求私钥错误
#define S_GET_PRIVATEKEY_ERR					0x00003004
//获取CSP信息错误
#define S_GETCSPINFO_ERR						0x00003005
//输入参数错误
#define S_PARAMETER_ERR							0x00003006



//数字信封加密错误
#define S_ENVELOPENC_ERR						0x00004001
//数字信封解密错误
#define S_ENVELOPDEC_ERR						0x00004002
//P7签名错误
#define S_PKCS7ENC_ERR							0x00004003
//P7验证错误
#define S_PKCS7DEC_ERR							0x00004004


extern 

/*****************************************************************************************************************************************
* 通过证书微缩图查找用户证书
* 返回值：0：成功，其他值：错误码
*
* 参数说明：
* ------------------------------------------------------------------------------------------
*      参数名称         参数类型                  输入/输出               说明
*
*    certPrm			  BYTE                       输入             证书微缩图
*   certPrmLen            int                        输入             证书微缩图数据长度
*     pCert               BYTE                       输出             二进制证书数据
*    pCertLen             int                        输出             二进制证书数据长度
* -------------------------------------------------------------------------------------------
*****************************************************************************************************************************************/
DECHNICCRYPT_API int _stdcall DECHNIC_GetCertByPrm(BYTE *certPrm, int certPrmLen,BYTE *pCert, int* pCertLen);

/*****************************************************************************************************************************************
 * PKCS7签名
 * 返回值：0：成功，其他值：错误码
 *
 * 参数说明：
 * ------------------------------------------------------------------------------------------
 *     参数名称         参数类型                  输入/输出               说明
 *
 *    pbCertData          BYTE                       输入             发送者的证书数据
 *   nCertDataLen         int                        输入             发送者的证书数据长度
 *    pbDataBuf           BYTE                       输入            要签名的原始数据
 *   nDataBufLen          int                        输入           要签名的原始数据长度
 * pbSignedDataBuf		  BYTE                       输出             P7签名后的数据
 * pnSignedDataLen        int                        输出           P7签名后的数据长度
 * -------------------------------------------------------------------------------------------
*****************************************************************************************************************************************/
//PKCS7签名
DECHNICCRYPT_API int _stdcall DECHNIC_PKCS7Sign(BYTE *pbCertData, int nCertDataLen, 
					BYTE *pbDataBuf, int nDataBufLen, 
					BYTE *pbSignedDataBuf, int *pnSignedDataLen);

/*****************************************************************************************************************************************
* PKCS7验签
* 返回值：0：成功，其他值：错误码
*
* 参数说明：
* -----------------------------------------------------------------------------------------------------------------
*      参数名称         参数类型                  输入/输出                 说明
*
*  pbSignedDataBuf        BYTE                       输入              P7签名后的数据
*   nSignedDataLen        int                        输入             P7签名后的数据长度
*     pbDataBuf           BYTE                       输出        从P7签名数据中解密出的原始数据
*    pnDataBufLen         int                        输出       从P7签名数据中解密出的原始数据长度
*  pchRootCertData        BYTE                       输入     根证书数据（可选，如果为空，不验证根证书）
* nRootCertDataLen        int                        输入    根证书数据长度（可选，如果为0，不验证根证书）
*    pchCRLData           BYTE                       输入        CRL数据（可选，如果为空，不验证CRL）
*   nCRLDataLen           int                        输入       CRL数据长度（可选，如果为0，不验证CRL）
* ------------------------------------------------------------------------------------------------------------------
*****************************************************************************************************************************************/
DECHNICCRYPT_API int _stdcall DECHNIC_PKCS7Verify(BYTE *pbSignedDataBuf, int nSignedDataLen, 
					  BYTE *pbDataBuf, int *pnDataBufLen, 
					  BYTE *pchSenderCertData, int *pnSenderCertDataLen, 
					  BYTE *pchRootCertData, int nRootCertDataLen, 
					  BYTE *pchCRLData, int nCRLDataLen);

/*****************************************************************************************************************************************
* 数字信封加密
* 返回值：0：成功，其他值：错误码
*
* 参数说明：
* ------------------------------------------------------------------------------------------
*     参数名称             参数类型                  输入/输出               说明
*
*    pbSenderCertData         BYTE                      输入             发送者的证书数据
*   nSenderCertDataLen        int                       输入           发送者的证书数据长度
*    pbRecipientCertData      BYTE                      输入             接收者的证书数据
*   nRecipientCertDataLen     int                       输入           接收者的证书数据长度
*    pbDataBuf                BYTE                      输入             要加密的原始数据
*    nDataLen                 int                       输入           要加密的原始数据长度
* pbEncodedDataBuf            BYTE                      输出              加密后的数据
* pnEncodedDataBufLen         int                       输出            加密后的数据长度
* -------------------------------------------------------------------------------------------
*****************************************************************************************************************************************/
DECHNICCRYPT_API int _stdcall DECHNIC_EncodeEnvelop(BYTE *pbSenderCertData, int nSenderCertDataLen, 
						BYTE *pbRecipientCertData, int nRecipientCertDataLen,  
						BYTE *pbDataBuf, int nDataLen, 
						BYTE *pbEncodedDataBuf, int *pnEncodedDataBufLen);


/*****************************************************************************************************************************************
* 数字信封解密
* 返回值：0：成功，其他值：错误码
*
* 参数说明：
* ------------------------------------------------------------------------------------------
*      参数名称         参数类型                  输入/输出               说明
*
*      pbEnvData		  BYTE                       输入               密文数据
*     nEnvDataLen         int                        输入             密文数据长度
*     pbDataBuf           BYTE                       输出             解密后的数据
*    pnDataBufLen         int                        输出            解密后的数据长度
* -------------------------------------------------------------------------------------------
*****************************************************************************************************************************************/
DECHNICCRYPT_API int _stdcall DECHNIC_DecodeEnvelop(
						BYTE *pbEnvData, int nEnvDataLen, 
						BYTE *pbDataBuf, int *pnDataBufLen);