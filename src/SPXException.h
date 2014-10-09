#include <exception>
#include <stdexcept>
#include <string>
#include <sstream>

//@TODO
// - Implement className, methodName, fileName, and lineNumber
// - (Maybe) Stack trace: return the entire stack trace as string to
//				give to a logging function?

//@TODO Why are all the string aruments not const std::string & ???

#ifndef SPXEXCEPTION_H
#define SPXEXCEPTION_H

class SPXException : public std::runtime_error {
public:
	SPXException() : std::runtime_error("Unknown Exception") {}
	
	virtual ~SPXException() throw() {}

	virtual const char * what() const throw() {
		return "SPXException: Unknown Exception";
	}
	
	/*
	virtual const char * what() const throw() {
		std::string detailedMessage;
		
		detailedMessage = filename + ": " + lineNumber + ": (" + className + "::" + methodName + ": " + message;
		return detailedMessage.c_str();
	}
	*/
	
private:
	std::string message;
	std::string className;
	std::string methodName;
	std::string fileName;
	unsigned int lineNumber;
};

class SPXINIParseException : public SPXException {
public:	
	SPXINIParseException(std::string section, std::string name, std::string message) : SPXException() {
		this->section = section;
		this->name = name;
		this->message = message;
	}
	
	SPXINIParseException(std::string section, std::string message) : SPXException() {
		this->section = section;
		this->name.clear();
		this->message = message;
	}
	
	~SPXINIParseException() throw() {}
	
	const char * what() const throw() {
		std::string tmp;
		tmp = "SPXINIParseException: Error parsing [" + section + "]:" + name + ": " + message;
		return tmp.c_str();
	}
	
private:
	std::string section;
	std::string name;
	std::string message;
};

class SPXFileIOException : public SPXException {
public:
	SPXFileIOException(std::string filename, std::string message) : SPXException() {
		this->filename = filename;
		this->message = message;
	}

	SPXFileIOException(const std::string & message) : SPXException() {
		this->filename.clear();
		this->message = message;
	}
	
	~SPXFileIOException() throw() {}
	
	const char * what() const throw() {
		std::string tmp;
		if(!filename.empty()) {
			tmp = "SPXFileIOException: Unable to process file: " + filename + ": " + message;
		} else {
			tmp = "SPXFileIOException: " + message;
		}
		
		return tmp.c_str();
	}
	
private:
	std::string filename;
	std::string message;
};

class SPXParseException : public SPXException {
public:
	SPXParseException(std::string message) : SPXException() {
		this->filename.clear();
		this->message = message;
	}
	
	SPXParseException(std::string filename, std::string message) : SPXException() {
		this->filename = filename;
		this->message = message;
	}
	
	~SPXParseException() throw() {}
	
	const char * what() const throw() {
		std::string tmp;
		
		if(filename.empty()) {
			tmp = "SPXParseException: " + message;
		} else {
			tmp = "SPXParseException: File: " + filename + ": " + message;
		}
		
		return tmp.c_str();
	}
	
private:
	std::string filename;
	std::string message;
};

class SPXOutOfRangeException : public SPXException {
public:
	SPXOutOfRangeException(int bottom, int top, int val, std::string message) : SPXException() {
		this->bottom = bottom;
		this->top = top;
		this->val = val;
		this->message = message;
	}
	
	SPXOutOfRangeException(int top, int val, std::string message) : SPXException() {
		this->bottom = 0;
		this->top = top;
		this->val = val;
		this->message = message;
	}
	
	~SPXOutOfRangeException() throw() {}
	
	const char * what() const throw() {

		std::ostringstream s;
		s << "SPXOutOfRangeException: Value: " << val << " outside of range: " << bottom << " <--> " << top << ": " << message;
		
		return s.str().c_str();
	}
	
private:
	int bottom;
	int top;
	int val;
	std::string message;
};

#endif
