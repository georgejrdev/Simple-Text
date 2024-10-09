#include "mainwindow.h"

#include <QApplication>
#include <QWidget>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QLabel>
#include <QTextStream>
#include <QMessageBox>
#include <QEvent>
#include <QCursor>
#include <QShortcut>
#include <QKeySequence>
#include <QLineEdit>
#include <QFont>

bool isMenuVisible = false;
QString currentFileName;
QLabel *fileNameLabel;

void toggleMenu(QWidget* menu, QWidget* parent) {
    if (isMenuVisible) {
        menu->hide();
    } else {
        menu->move(parent->pos().x() + 10, parent->pos().y() + 40);
        menu->show();
        menu->raise();
    }
    isMenuVisible = !isMenuVisible;
}

void openFile(QTextEdit *textEdit) {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open file", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) return;

    currentFileName = fileName;
    fileNameLabel->setText(QFileInfo(fileName).fileName());

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, "Error", "Unable to open file.");
        return;
    }

    QTextStream in(&file);
    textEdit->setPlainText(in.readAll());
    file.close();
}

void saveFile(QTextEdit *textEdit) {
    if (currentFileName.isEmpty()) {
        QString fileName = QFileDialog::getSaveFileName(nullptr, "Save file", "", "Text Files (*.txt);;All Files (*)");
        if (fileName.isEmpty()) return;

        currentFileName = fileName;
    }

    QFile file(currentFileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(nullptr, "Error", "Unable to save file.");
        return;
    }

    QTextStream out(&file);
    out << textEdit->toPlainText();
    file.close();

    fileNameLabel->setText(QFileInfo(currentFileName).fileName());
}

void updateFontSize(QTextEdit *textEdit, QLineEdit *fontSizeInput, int change = 0) {
    bool ok;
    int currentSize = fontSizeInput->text().toInt(&ok);
    if (!ok) return;

    int newSize = currentSize + change;
    if (newSize < 1) return;

    QFont font = textEdit->font();
    font.setPointSize(newSize);
    textEdit->setFont(font);

    fontSizeInput->setText(QString::number(newSize));
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Simple Text");
    window.resize(600, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(&window);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget *topBar = new QWidget;
    topBar->setFixedHeight(40);
    topBar->setStyleSheet("background-color: #464646;");

    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(0, 0, 0, 0);
    topBarLayout->setSpacing(0);

    QPushButton *button = new QPushButton("● ● ●");
    button->setStyleSheet("color: #FF7474; background-color: transparent; border: none; font-size: 10px;");
    button->setFixedWidth(60);

    fileNameLabel = new QLabel("New File");
    fileNameLabel->setStyleSheet("color: #FFFFFF; font-size: 16px;");
    fileNameLabel->setAlignment(Qt::AlignCenter);

    topBarLayout->addWidget(button, 0, Qt::AlignLeft);
    topBarLayout->addWidget(fileNameLabel, 1);
    mainLayout->addWidget(topBar);

    QWidget *menu = new QWidget(&window);
    menu->setFixedSize(150, 150);
    menu->setStyleSheet("background-color: #2F2F2F;");
    menu->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    menu->setVisible(false);

    QVBoxLayout *menuLayout = new QVBoxLayout(menu);
    menuLayout->setContentsMargins(0, 0, 0, 0);
    menuLayout->setSpacing(5);

    QPushButton *openButton = new QPushButton("Open");
    openButton->setStyleSheet("background-color: #000000; color: #FFFFFF; font-size: 12px;");
    openButton->setFixedHeight(30);

    QPushButton *saveButton = new QPushButton("Save");
    saveButton->setStyleSheet("background-color: #000000; color: #FFFFFF; font-size: 12px;");
    saveButton->setFixedHeight(30);

    QLabel *fontSizeLabel = new QLabel("Font Size");
    fontSizeLabel->setStyleSheet("color: #FFFFFF; font-size: 12px;");
    fontSizeLabel->setAlignment(Qt::AlignCenter);

    QWidget *fontSizeWidget = new QWidget;
    QHBoxLayout *fontSizeLayout = new QHBoxLayout(fontSizeWidget);
    fontSizeLayout->setContentsMargins(0, 0, 0, 0);
    fontSizeLayout->setSpacing(5);

    QPushButton *decreaseFontButton = new QPushButton("-");
    decreaseFontButton->setFixedSize(30, 30);
    decreaseFontButton->setStyleSheet("background-color: #000000; color: #FFFFFF; font-size: 12px;");

    QLineEdit *fontSizeInput = new QLineEdit("12");
    fontSizeInput->setFixedSize(40, 30);
    fontSizeInput->setStyleSheet("background-color: #FFFFFF; color: #000000; text-align: center;");
    fontSizeInput->setAlignment(Qt::AlignCenter);

    QPushButton *increaseFontButton = new QPushButton("+");
    increaseFontButton->setFixedSize(30, 30);
    increaseFontButton->setStyleSheet("background-color: #000000; color: #FFFFFF; font-size: 12px;");

    fontSizeLayout->addWidget(decreaseFontButton);
    fontSizeLayout->addWidget(fontSizeInput);
    fontSizeLayout->addWidget(increaseFontButton);

    menuLayout->addWidget(openButton);
    menuLayout->addWidget(saveButton);
    menuLayout->addWidget(fontSizeLabel);
    menuLayout->addWidget(fontSizeWidget);

    QTextEdit *textEdit = new QTextEdit;
    mainLayout->addWidget(textEdit);

    QObject::connect(openButton, &QPushButton::clicked, [&]() {
        openFile(textEdit);
        toggleMenu(menu, &window);
    });

    QObject::connect(saveButton, &QPushButton::clicked, [&]() {
        saveFile(textEdit);
        toggleMenu(menu, &window);
    });

    QObject::connect(button, &QPushButton::clicked, [&]() {
        toggleMenu(menu, &window);
    });

    QObject::connect(decreaseFontButton, &QPushButton::clicked, [&]() {
        updateFontSize(textEdit, fontSizeInput, -1);
    });

    QObject::connect(increaseFontButton, &QPushButton::clicked, [&]() {
        updateFontSize(textEdit, fontSizeInput, 1);
    });

    QObject::connect(fontSizeInput, &QLineEdit::textChanged, [&]() {
        updateFontSize(textEdit, fontSizeInput);
    });

    QFont initialFont = textEdit->font();
    initialFont.setPointSize(12);
    textEdit->setFont(initialFont);

    window.setLayout(mainLayout);
    window.show();

    QShortcut *openShortcut = new QShortcut(QKeySequence("Ctrl+O"), &window);
    QObject::connect(openShortcut, &QShortcut::activated, [&]() {
        openFile(textEdit);
    });

    QShortcut *saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), &window);
    QObject::connect(saveShortcut, &QShortcut::activated, [&]() {
        saveFile(textEdit);
    });

    return app.exec();
}
