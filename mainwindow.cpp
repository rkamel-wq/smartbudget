#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QMessageBox>

void MainWindow::handleServerMessage(const QString& message)
{
    QJsonParseError parseError;

    QJsonDocument doc = QJsonDocument::fromJson(
        message.toUtf8(),
        &parseError
        );

    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, "Server Error", "Invalid response from server.");
        return;
    }

    if (!doc.isObject()) {
        QMessageBox::warning(this, "Server Error", "Server response was not a JSON object.");
        return;
    }

    QJsonObject root = doc.object();

    QString status = root["status"].toString();

    if (status == "error") {
        QString errorMessage = root["message"].toString("Unknown server error.");
        QMessageBox::warning(this, "Error", errorMessage);
        return;
    }

    if (status == "ok") {
        if (root.contains("message")) {
            QString okMessage = root["message"].toString();
            qDebug() << "Server OK:" << okMessage;
        }

        if (root.contains("transactions") && root["transactions"].isArray()) {
            QJsonArray transactionsArray = root["transactions"].toArray();



            for (const QJsonValue& value : transactionsArray) {
                if (!value.isObject())
                    continue;

                QJsonObject tx = value.toObject();

                double amount = tx["amount"].toDouble();
                QString type = tx["type"].toString();
                QString category = tx["category"].toString();
                QString date = tx["date"].toString();


            }
        }

        return;
    }

    QMessageBox::warning(this, "Server Error", "Unknown server response status.");
}