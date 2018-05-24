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

//�ɹ�
#define DECHNIC_CRYPT_SUCCESS                        0x0
//�Ƿ��Ĳ���
#define DECHNIC_CRYPT_INVALID_PARA                   0x00002001
//��ȡ֤�����ݴ���
#define DECHNIC_CRYPT_GET_CERT_DATA                  0x00002002
//ǩ���������
#define DECHNIC_CRYPT_SIGN_DATA                      0x00002003
//��ǩ�������
#define DECHNIC_CRYPT_VERIFY_DATA                    0x00002004
//ǩ����֤ʧ��
#define DECHNIC_CRYPT_BAD_SIGNATURE                  0x00002005
//�����������
#define DECHNIC_CRYPT_ENCRYPT_DATA                   0x00002006
//�����������
#define DECHNIC_CRYPT_DECRYPT_DATA                   0x00002007
//������̫С
#define DECHNIC_CRYPT_MORE_DATA                      0x00002008
//δ�ҵ�����֤��
#define DECHNIC_CRYPT_INVALID_CERT                   0x00002009
//�ڴ治��
#define DECHNIC_CRYPT_MALLOC_ERR                     0x0000200A
//֤�����δ��֤
#define DECHNIC_CRYPT_NO_CHECK_PIN_ERR               0x0000200E



//base64 �������
#define S_BASE64ENCODE_ERR						0x00003001
//base64 �������
#define S_BASE64DECODE_ERR						0x00003002
//����ռ����
#define S_MALLOC_ERR							0x00003003
//����˽Կ����
#define S_GET_PRIVATEKEY_ERR					0x00003004
//��ȡCSP��Ϣ����
#define S_GETCSPINFO_ERR						0x00003005
//�����������
#define S_PARAMETER_ERR							0x00003006



//�����ŷ���ܴ���
#define S_ENVELOPENC_ERR						0x00004001
//�����ŷ���ܴ���
#define S_ENVELOPDEC_ERR						0x00004002
//P7ǩ������
#define S_PKCS7ENC_ERR							0x00004003
//P7��֤����
#define S_PKCS7DEC_ERR							0x00004004


extern 

/*****************************************************************************************************************************************
* ͨ��֤��΢��ͼ�����û�֤��
* ����ֵ��0���ɹ�������ֵ��������
*
* ����˵����
* ------------------------------------------------------------------------------------------
*      ��������         ��������                  ����/���               ˵��
*
*    certPrm			  BYTE                       ����             ֤��΢��ͼ
*   certPrmLen            int                        ����             ֤��΢��ͼ���ݳ���
*     pCert               BYTE                       ���             ������֤������
*    pCertLen             int                        ���             ������֤�����ݳ���
* -------------------------------------------------------------------------------------------
*****************************************************************************************************************************************/
DECHNICCRYPT_API int _stdcall DECHNIC_GetCertByPrm(BYTE *certPrm, int certPrmLen,BYTE *pCert, int* pCertLen);

/*****************************************************************************************************************************************
 * PKCS7ǩ��
 * ����ֵ��0���ɹ�������ֵ��������
 *
 * ����˵����
 * ------------------------------------------------------------------------------------------
 *     ��������         ��������                  ����/���               ˵��
 *
 *    pbCertData          BYTE                       ����             �����ߵ�֤������
 *   nCertDataLen         int                        ����             �����ߵ�֤�����ݳ���
 *    pbDataBuf           BYTE                       ����            Ҫǩ����ԭʼ����
 *   nDataBufLen          int                        ����           Ҫǩ����ԭʼ���ݳ���
 * pbSignedDataBuf		  BYTE                       ���             P7ǩ���������
 * pnSignedDataLen        int                        ���           P7ǩ��������ݳ���
 * -------------------------------------------------------------------------------------------
*****************************************************************************************************************************************/
//PKCS7ǩ��
DECHNICCRYPT_API int _stdcall DECHNIC_PKCS7Sign(BYTE *pbCertData, int nCertDataLen, 
					BYTE *pbDataBuf, int nDataBufLen, 
					BYTE *pbSignedDataBuf, int *pnSignedDataLen);

/*****************************************************************************************************************************************
* PKCS7��ǩ
* ����ֵ��0���ɹ�������ֵ��������
*
* ����˵����
* -----------------------------------------------------------------------------------------------------------------
*      ��������         ��������                  ����/���                 ˵��
*
*  pbSignedDataBuf        BYTE                       ����              P7ǩ���������
*   nSignedDataLen        int                        ����             P7ǩ��������ݳ���
*     pbDataBuf           BYTE                       ���        ��P7ǩ�������н��ܳ���ԭʼ����
*    pnDataBufLen         int                        ���       ��P7ǩ�������н��ܳ���ԭʼ���ݳ���
*  pchRootCertData        BYTE                       ����     ��֤�����ݣ���ѡ�����Ϊ�գ�����֤��֤�飩
* nRootCertDataLen        int                        ����    ��֤�����ݳ��ȣ���ѡ�����Ϊ0������֤��֤�飩
*    pchCRLData           BYTE                       ����        CRL���ݣ���ѡ�����Ϊ�գ�����֤CRL��
*   nCRLDataLen           int                        ����       CRL���ݳ��ȣ���ѡ�����Ϊ0������֤CRL��
* ------------------------------------------------------------------------------------------------------------------
*****************************************************************************************************************************************/
DECHNICCRYPT_API int _stdcall DECHNIC_PKCS7Verify(BYTE *pbSignedDataBuf, int nSignedDataLen, 
					  BYTE *pbDataBuf, int *pnDataBufLen, 
					  BYTE *pchSenderCertData, int *pnSenderCertDataLen, 
					  BYTE *pchRootCertData, int nRootCertDataLen, 
					  BYTE *pchCRLData, int nCRLDataLen);

/*****************************************************************************************************************************************
* �����ŷ����
* ����ֵ��0���ɹ�������ֵ��������
*
* ����˵����
* ------------------------------------------------------------------------------------------
*     ��������             ��������                  ����/���               ˵��
*
*    pbSenderCertData         BYTE                      ����             �����ߵ�֤������
*   nSenderCertDataLen        int                       ����           �����ߵ�֤�����ݳ���
*    pbRecipientCertData      BYTE                      ����             �����ߵ�֤������
*   nRecipientCertDataLen     int                       ����           �����ߵ�֤�����ݳ���
*    pbDataBuf                BYTE                      ����             Ҫ���ܵ�ԭʼ����
*    nDataLen                 int                       ����           Ҫ���ܵ�ԭʼ���ݳ���
* pbEncodedDataBuf            BYTE                      ���              ���ܺ������
* pnEncodedDataBufLen         int                       ���            ���ܺ�����ݳ���
* -------------------------------------------------------------------------------------------
*****************************************************************************************************************************************/
DECHNICCRYPT_API int _stdcall DECHNIC_EncodeEnvelop(BYTE *pbSenderCertData, int nSenderCertDataLen, 
						BYTE *pbRecipientCertData, int nRecipientCertDataLen,  
						BYTE *pbDataBuf, int nDataLen, 
						BYTE *pbEncodedDataBuf, int *pnEncodedDataBufLen);


/*****************************************************************************************************************************************
* �����ŷ����
* ����ֵ��0���ɹ�������ֵ��������
*
* ����˵����
* ------------------------------------------------------------------------------------------
*      ��������         ��������                  ����/���               ˵��
*
*      pbEnvData		  BYTE                       ����               ��������
*     nEnvDataLen         int                        ����             �������ݳ���
*     pbDataBuf           BYTE                       ���             ���ܺ������
*    pnDataBufLen         int                        ���            ���ܺ�����ݳ���
* -------------------------------------------------------------------------------------------
*****************************************************************************************************************************************/
DECHNICCRYPT_API int _stdcall DECHNIC_DecodeEnvelop(
						BYTE *pbEnvData, int nEnvDataLen, 
						BYTE *pbDataBuf, int *pnDataBufLen);