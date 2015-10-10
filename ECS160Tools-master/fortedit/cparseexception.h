#ifndef CPARSEEXCEPTION_H
#define CPARSEEXCEPTION_H

#include <QException>
#include <QString>

#include <string>

using std::string;

/**
 * Extended exception to signify a parsing exception
 */
class CParseException : public QException
{
public:
    CParseException(const char* msg)
    {
        DMessage = msg;
    }

    void raise() const
    {
        throw *this;
    }

    CParseException* clone() const
    {
        CParseException* E = new CParseException(DMessage);
        return E;
    }

    QString Message() const
    {
        return QString(DMessage);
    }

private:
    const char* DMessage;
};

#endif // CPARSEEXCEPTION_H
