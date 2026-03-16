#ifndef RECOMMENDER_EXCEPTIONS_H
#define RECOMMENDER_EXCEPTIONS_H

#include <exception>
#include <string>

class RecommenderException : public std::exception {

	protected:

	std::string exceptionName = "RecommenderException";
	std::string exceptionMessage;
    std::exception* exceptionCause = nullptr;
	bool causeIsRecommenderException = false;

	public:

	std::string getExceptionMessage(){
		return exceptionMessage;
	}

	virtual std::string getExceptionName(){
		return exceptionName;
	}

	std::exception* getExceptionCause(){
		return exceptionCause;
	}
        
        bool isCauseRecommenderException(){
            return causeIsRecommenderException;
        }
};

//for rare/unexpected cases where parameters have invalid values/states after validation.
class InvalidStateException : public RecommenderException {

    protected:

        std::string exceptionName = "InvalidStateException";

    public:

        //create with default message.
        InvalidStateException(){
            exceptionMessage = "The application is in an invalid state.";
        }

        //create with user-defined message.
        InvalidStateException(std::string message){
            exceptionMessage = "The application is in an invalid state: "+message;
        }

        //overrides
        std::string getExceptionName(){
            return exceptionName;
        }
};

class UserNotFoundException : public RecommenderException {
	
        private:
        unsigned long exceptionUserId = 0;

	protected:

	std::string exceptionName = "UserNotFoundException";

	public:
	
	//create with default message.
	UserNotFoundException() {
		exceptionMessage = "User not found.";
	}
	
	//create with user-defined message.
	UserNotFoundException(std::string message){
		exceptionMessage = message;
	}

        //create with user-defined message and userId
        UserNotFoundException(std::string message,unsigned long userId){
                exceptionMessage = message;
                exceptionUserId = userId;
        }

	//create with user-defined message and recommender cause
	UserNotFoundException(std::string message, RecommenderException* cause){
		exceptionMessage = message;
		exceptionCause = cause;
		causeIsRecommenderException = true;
	}

        //create with user-defined message,recommender cause, and userId
        UserNotFoundException(std::string message, RecommenderException* cause,unsigned long userId){
                exceptionMessage = message;
                exceptionCause = cause;
                exceptionUserId = userId;
        }

        //if it doesnt exist, zero will be returned.
        unsigned long getExceptionUserIdIfExists(){
            return exceptionUserId;
        }

	//overrides
	std::string getExceptionName(){
		return exceptionName;
	}
};


class MovieNotFoundException : public RecommenderException {

	protected:

	std::string exceptionName = "MovieNotFoundException";
		
	public:
	
	//create with default message.
	MovieNotFoundException() {
		exceptionMessage = "Movie not found.";
	}
	
	//create with user-defined message.
	MovieNotFoundException(std::string message){
		exceptionMessage = message;
	}

	//create with user-defined message and recommender cause
	MovieNotFoundException(std::string message, RecommenderException* cause){
		exceptionMessage = message;
		exceptionCause = cause;
		causeIsRecommenderException = true;
	}

	//overrides
	std::string getExceptionName(){
		return exceptionName;
	}
};

class UserProfileNotFoundException : public RecommenderException {

    protected:

    std::string exceptionName = "UserProfileNotFoundException";

    public:

    //create with default message.
    UserProfileNotFoundException(){
        exceptionMessage = "User profile not found.";
    }

    //create with user-defined message.
    UserProfileNotFoundException(std::string message){
        exceptionMessage = message;
    }
};

class RecJsonParsingException : public RecommenderException {

	protected:

	std::string exceptionName = "RecJsonParsingException";

	public:

	//create with default message.
	RecJsonParsingException() {
		exceptionMessage = "Error while parsing json.";
	}

	//create with user-defined message.
	RecJsonParsingException(std::string message){
		exceptionMessage = message;
	}

	//create with generic cause
	RecJsonParsingException(std::exception* cause){
		exceptionCause = cause;
		exceptionMessage = cause->what();
	}

	//overrides
	std::string getExceptionName(){
		return exceptionName;
	}

};

class RecCsvParsingException : public RecommenderException {

	protected:

	std::string exceptionName = "RecCsvParsingException";

	public:

	//create with default message.
	RecCsvParsingException() {
		exceptionMessage = "Error while parsing csv.";
	}

	//create with user-defined message.
	RecCsvParsingException(std::string message){
		exceptionMessage = message;
	}

	//create with cause
	RecCsvParsingException(std::exception* cause){
		exceptionCause = cause;
		exceptionMessage = cause->what();
	}

	//overrides
	std::string getExceptionName(){
		return exceptionName;
	}
};

class RecJsonWriteException : public RecommenderException {

	protected:

	std::string exceptionName = "RecJsonWriteException";

	public:

	//create with default message.
	RecJsonWriteException(){
		exceptionMessage = "Error while writing json.";
	}

	//create with user-defined message.
	RecJsonWriteException(std::string message){
		exceptionMessage = message;
	}

	//overrides
	std::string getExceptionName(){
		return exceptionName;
	}
};

class RecCsvWriteException : public RecommenderException {

	protected:

	std::string exceptionName = "RecCsvWriteException";

	public:

	//create with default message.
	RecCsvWriteException(){
		exceptionMessage = "Error while writing csv.";
	}

	//create with user-defined message.
	RecCsvWriteException(std::string message){
		exceptionMessage = message;
	}

	//overrides
	std::string getExceptionName(){
		return exceptionName;
	}
};

class NotYetImplementedException : public RecommenderException {

    protected:

    std::string exceptionName = "NotYetImplementedException";

    public:

    //create with default message
    NotYetImplementedException(std::string functionalityName){
        exceptionMessage = "This functionality is not yet implemented. functionality="+functionalityName;
    }

    //overrides
    std::string getExceptionName(){
        return exceptionName;
    }
};

class InvalidCmdLineArgumentException: public RecommenderException {

    protected:
    std::string exceptionName = "InvalidCmdLineArgumentException";

    public:
    //create with default message
    InvalidCmdLineArgumentException(std::string unrecognizedArg){
        exceptionMessage = "Invalid/unrecognized command line argument. argument="+unrecognizedArg;
    }

    //overrides
    std::string getExceptionName(){
        return exceptionName;
    }
};

#endif
