///////////////////实现文件
#include "stdafx.h"
#include "XML.h"

int CXML::ParseXmlFile(CString xmlFile)
{
	int result=0;
	try
	{
		if(m_xml.LoadFile(xmlFile))
			result=1;
		else
			result=0;
	}
	catch(...)
	{
	}
	return result;
}

int CXML::ParseXmlStr(CString xmlStr)

{
	int result=0;
	if(xmlStr=="")
		return 0;
	try
	{
		if(m_xml.Parse(xmlStr))
			result=1;
		else
			result=0;
	}
	catch(...)
	{
	}
	return result;
}

int CXML::getElementValue(TiXmlElement* pcrElement,CString ElementMark,CString& value)
{
	int result=0;
	if(ElementMark=="" || pcrElement==NULL)
		return 0;
	try
	{
		pcrElement=getElement(pcrElement,ElementMark);
		if(pcrElement)
		{
			m_pElement=pcrElement;
			value=m_pElement->GetText();
			result=1;
		}
	}
	catch(...){}
	return result;
}


TiXmlElement* CXML::getElement(TiXmlElement* pcrElement, CString ElementMark)
{
	TiXmlElement* pElementtmp=NULL;
	pElementtmp=pcrElement;
	while(pElementtmp)
	{
		if(strcmp(pElementtmp->Value(),ElementMark)==0)
		{
			//printf("%s\r\n",pElementtmp->Value());
			return pElementtmp;
		}
		else
		{
			TiXmlElement* nextElement=pElementtmp->FirstChildElement();
			while(nextElement)
			{
				//printf("%s\r\n",nextElement->Value());
				if(strcmp(nextElement->Value(),ElementMark)==0)
				{
					return nextElement;
				}
				else
				{
					TiXmlElement* reElement=NULL;
					reElement=getElement(nextElement,ElementMark);
					if(reElement)
					{
						return reElement;
					}
				}
				nextElement=nextElement->NextSiblingElement();
			}
		}
		pElementtmp=pElementtmp->NextSiblingElement();
	}
	return NULL;
}

TiXmlElement* CXML::getNextElement(TiXmlElement* pcrElement)
{
	if(pcrElement)
		return pcrElement->NextSiblingElement();
	return NULL;
}

TiXmlElement* CXML::getElementByMarkAttribute(TiXmlElement* pcrElement, CString strElementMark,CString strAttribute, CString strValue)
{
	TiXmlElement* pChild =  getElement(pcrElement,strElementMark);
	CString strHaveVal;
	while(pChild)
	{
		 strHaveVal = pChild->Attribute(strAttribute);
		 if(strHaveVal==strValue)
			 return pChild;
		 else{
			 pChild = pChild->NextSiblingElement(strElementMark);
		 }
	}
	return NULL;
}

//根据标签取值

int CXML::getFirstElementValue(CString ElementMark,CString& value)
{
	int result=0;
	if(ElementMark=="")
		return 0;
	try
	{
		TiXmlElement* pcrElement=NULL;
		pcrElement=m_xml.RootElement();
		pcrElement=getElement(pcrElement,ElementMark);
		if(pcrElement)
		{
			m_pElement=pcrElement;
			value=m_pElement->GetText();
			result=1;
		}
	}
	catch(...){}
	return result;
}

int CXML::getNextElementValue(CString ElementMark,CString& value)
{
	value="";
	try{
		m_pElement=m_pElement->NextSiblingElement(ElementMark);
		if(m_pElement)
		{
			value=m_pElement->GetText();
			return 1;
		}
	}
	catch(...){}
	return 0;
}

CString CXML::getXmlStr()
{
	CString result="";
	try
	{
		TiXmlPrinter printer;
		m_xml.Accept(&printer);
		result=printer.CStr();
	}
	catch(...)
	{
	}
	return result;
}

void CXML::Clear()
{
	m_xml.Clear();
}

//添加子节点
TiXmlElement* CXML::addXmlRootElement(CString ElementMark)
{
	TiXmlElement* RootElement=new TiXmlElement(ElementMark);
	m_xml.LinkEndChild(RootElement);
	return RootElement;
}

TiXmlElement* CXML::addXmlChildElement(TiXmlElement* pElement,CString ElementMark)
{
	if(pElement)
	{
		TiXmlElement* tempElement=new TiXmlElement(ElementMark);
		pElement->LinkEndChild(tempElement);
		return tempElement;
	}
	return 0;
}

void CXML::addElementValue(TiXmlElement *pElement, CString value)
{
	if(pElement)
	{
		TiXmlText *pContent=new TiXmlText(value);
		pElement->LinkEndChild(pContent);
	}
}

//添加属性及属性值

void CXML::addXmlAttribute(TiXmlElement* pElement,CString AttributeMark,CString value)
{
	if(pElement)
	{
		pElement->SetAttribute(AttributeMark,value);
	}
}

//添加声明

void CXML::addXmlDeclaration(CString vesion,CString encoding,CString standalone)
{
	TiXmlDeclaration *pDeclaration=new TiXmlDeclaration(vesion,encoding,standalone);
	m_xml.LinkEndChild(pDeclaration);
}

//添加注释

void CXML::addXmlComment(TiXmlElement* pElement,CString Comment)
{
	if(pElement)
	{
		TiXmlComment *pComment=new TiXmlComment(Comment);
		pElement->LinkEndChild(pComment);
	}
}

TiXmlElement* CXML::getRootElement()
{
	return m_xml.RootElement();
}

//取得属性值

int CXML::getElementAttributeValue(TiXmlElement* pElement,CString AttributeName,CString& value)
{
	if(pElement)
	{
		if(pElement->Attribute(AttributeName))
		{
			value=pElement->Attribute(AttributeName);
			return 1;
		}
	}
	return 0;
}

void CXML::saveFile(CString FileName)
{
	m_xml.SaveFile(FileName);
}
//////////////////////////////////////////