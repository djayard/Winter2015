#ifndef CMAPEXCEPTION_H
#define CMAPEXCEPTION_H
#include <QException>
#include <QString>

#include <string>

using std::string;

/**
 * Extended exception to signify a map error
 */
class CMapException : public QException
{
public:
    CMapException(const char* msg)
    {
        DMessage = msg;
    }

    void raise() const
    {
        throw *this;
    }

    CMapException* clone() const
    {
        CMapException* E = new CMapException(DMessage);
        return E;
    }

    QString Message() const
    {
        return QString(DMessage);
    }

private:
    const char* DMessage;
};

#endif // CMAPEXCEPTION_H
