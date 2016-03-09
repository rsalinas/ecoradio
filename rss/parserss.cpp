#include <QtXml/QtXml>
#include <iostream>

class MyXmlContentHandler :public QXmlDefaultHandler
{
    int indent = 0;
  public:
    std::ostream &printIndent() {
		return std::cout << std::string(indent*4, ' ');
}
	MyXmlContentHandler():QXmlDefaultHandler()
	{

	};
	~MyXmlContentHandler()
	{

	};

	bool startElement(const QString & namespaceURI, const QString & localName, 
					const QString & qName, const QXmlAttributes & atts ) override
	{
		printIndent() << "Read Start Tag : " << localName.toStdString()<< std::endl;		
		printIndent() << "Tag Attributes: " << std::endl;
		
		
		for(int index = 0 ; index < atts.length();index++)
		{
		  printIndent() << atts.type(index).toStdString()<< "="
		  << atts.value(index).toStdString()<< std::endl; 
		}
		
		printIndent() << "------------------------" << std::endl; 
		indent ++;
		return true;
	};
	bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName) override {
		indent--;;
	}
};

int main(int argc, char *argv[])
{

    QXmlSimpleReader* parser 		= new QXmlSimpleReader();
    MyXmlContentHandler* handler 	= new MyXmlContentHandler();
    

    parser->setContentHandler(handler);
    
    std::cout<<	"Starting parsing"<< std::endl;
    
    if(parser->parse(new QXmlInputSource(new QFile(argv[1]))))
    {
	std::cout<<"Parsed Successfully!"<< std::endl;
    }
    else
    {
	std::cout<<"Parsing Failed..."<< std::endl;
    }	
}
