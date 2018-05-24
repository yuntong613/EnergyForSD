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
	//解析xml字符串
	int ParseXmlStr(CString xmlstr);
	//解析xml文件
	int ParseXmlFile(CString xmlFile);
	//获取某一子节点下的节点信息
	int getElementValue(TiXmlElement* pcrElement,CString ElementMark,CString& value);
	//根据标签取值
	int getFirstElementValue(CString ElementMark,CString& value);
	//针对同一标签的记录取值,如果返回值是0表明再无此标签内容值可取
	int getNextElementValue(CString ElementMark,CString& value);
	//取得属性值
	int getElementAttributeValue(TiXmlElement* pElement,CString AttributeName,CString& value);
	//获取根结点
	TiXmlElement* getRootElement();
	//返回当前的xml字符串
	CString getXmlStr();
	//清空解析的内容
	void Clear();
	//添加子节点
	TiXmlElement* addXmlRootElement(CString ElementMark);//添加一个根节点
	//添加子节点
	TiXmlElement* addXmlChildElement(TiXmlElement* pElement,CString ElementMark);
	//给节点添加值
	void addElementValue(TiXmlElement* pElement,CString value);
	//添加属性及属性值
	void addXmlAttribute(TiXmlElement* pElement,CString AttributeMark,CString value);
	//添加声明
	void addXmlDeclaration(CString vesion,CString encoding,CString standalone);
	//添加注释
	void addXmlComment(TiXmlElement* pElement,CString Comment);
	//将xml内容保存到文件
	void saveFile(CString FileName);
};
