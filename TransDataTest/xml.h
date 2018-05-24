#include "tinyxml.h"

class CXML
{
public:
	CXML(void)
	{
	}
	~CXML(void)
	{
	}
private:
	TiXmlDocument m_xml;
	TiXmlElement* m_pElement;
public:
	TiXmlElement* getElement(TiXmlElement* pcrElement, CString ElementMark);
	TiXmlElement* getNextElement(TiXmlElement* pcrElement);
	TiXmlElement* getElementByMarkAttribute(TiXmlElement* pcrElement, CString strElementMark,CString strAttribute, CString strValue);
public:
	//����xml�ַ���
	int ParseXmlStr(CString xmlstr);
	//����xml�ļ�
	int ParseXmlFile(CString xmlFile);
	//��ȡĳһ�ӽڵ��µĽڵ���Ϣ
	int getElementValue(TiXmlElement* pcrElement,CString ElementMark,CString& value);
	//���ݱ�ǩȡֵ
	int getFirstElementValue(CString ElementMark,CString& value);
	//���ͬһ��ǩ�ļ�¼ȡֵ,�������ֵ��0�������޴˱�ǩ����ֵ��ȡ
	int getNextElementValue(CString ElementMark,CString& value);
	//ȡ������ֵ
	int getElementAttributeValue(TiXmlElement* pElement,CString AttributeName,CString& value);
	//��ȡ�����
	TiXmlElement* getRootElement();
	//���ص�ǰ��xml�ַ���
	CString getXmlStr();
	//��ս���������
	void Clear();
	//����ӽڵ�
	TiXmlElement* addXmlRootElement(CString ElementMark);//���һ�����ڵ�
	//����ӽڵ�
	TiXmlElement* addXmlChildElement(TiXmlElement* pElement,CString ElementMark);
	//���ڵ����ֵ
	void addElementValue(TiXmlElement* pElement,CString value);
	//������Լ�����ֵ
	void addXmlAttribute(TiXmlElement* pElement,CString AttributeMark,CString value);
	//�������
	void addXmlDeclaration(CString vesion,CString encoding,CString standalone);
	//���ע��
	void addXmlComment(TiXmlElement* pElement,CString Comment);
	//��xml���ݱ��浽�ļ�
	void saveFile(CString FileName);
};
