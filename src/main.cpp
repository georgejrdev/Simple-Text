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
#include <QCheckBox>

bool isMenuVisible = false;
QString currentFileName;
QLabel *fileNameLabel;
bool isFileModified = false;
QString version = "v1.0.1";


void toggleMenu(QWidget* menu) {
    if (isMenuVisible) {
        menu->hide();
    } else {
        menu->show();
        menu->raise();
    }
    isMenuVisible = !isMenuVisible;
}


void updateFileNameLabel() {
    QString displayName = QFileInfo(currentFileName).fileName();
    if (isFileModified) {
        displayName += " *";
    }
    fileNameLabel->setText(displayName);
}


void openFile(QTextEdit *textEdit) {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open file", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) return;

    currentFileName = fileName;
    isFileModified = false;
    updateFileNameLabel();

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

    isFileModified = false;
    updateFileNameLabel();
}


void saveAsNewFile(QTextEdit *textEdit){
    currentFileName.clear();
    saveFile(textEdit);
}


void loadFileFromArguments(int argc, char *argv[], QTextEdit *textEdit) {
    if (argc > 1) {
        QString fileName = argv[1];
        currentFileName = fileName;
        isFileModified = false;
        updateFileNameLabel();

        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            textEdit->setPlainText(in.readAll());
            file.close();
        } else {
            QMessageBox::warning(nullptr, "Error", "Unable to open file.");
        }
    }
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


void autoSaveFile(QTextEdit *textEdit, QCheckBox *autoSaveCheckBox) {
    if (autoSaveCheckBox->isChecked() && !currentFileName.isEmpty()) {
        saveFile(textEdit);
    }
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
    topBarLayout->setContentsMargins(10, 0, 10, 0);
    topBarLayout->setSpacing(0);

    QPushButton *button = new QPushButton("● ● ●");
    button->setStyleSheet("color: #FF7474; background-color: transparent; border: none; font-size: 10px;");
    button->setFixedWidth(60);

    fileNameLabel = new QLabel("New File");
    fileNameLabel->setStyleSheet("color: #FFFFFF; font-size: 16px;");
    fileNameLabel->setAlignment(Qt::AlignCenter);

    QLabel *versionLabel = new QLabel(version);
    versionLabel->setStyleSheet("color: #AAAAAA; font-size: 12px;");
    versionLabel->setFixedWidth(50);

    topBarLayout->addWidget(button, 0, Qt::AlignLeft);
    topBarLayout->addWidget(fileNameLabel, 1, Qt::AlignHCenter);
    topBarLayout->addWidget(versionLabel, 0, Qt::AlignRight);
    mainLayout->addWidget(topBar);


    QWidget *menu = new QWidget(&window);
    menu->setFixedSize(150, 250);
    menu->setGeometry(10, 50, 150, 200);
    menu->setStyleSheet("background-color: #2F2F2F;");
    menu->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    menu->setVisible(false);

    QVBoxLayout *menuLayout = new QVBoxLayout(menu);
    menuLayout->setContentsMargins(10, 10, 10, 10);
    menuLayout->setSpacing(5);

    QPushButton *openButton = new QPushButton("Open");
    openButton->setStyleSheet("background-color: #000000; color: #FFFFFF; font-size: 12px;");
    openButton->setFixedHeight(30);

    QPushButton *saveButton = new QPushButton("Save");
    saveButton->setStyleSheet("background-color: #000000; color: #FFFFFF; font-size: 12px;");
    saveButton->setFixedHeight(30);

    QPushButton *saveAsNewFileButton = new QPushButton("Save As");
    saveAsNewFileButton->setStyleSheet("background-color: #000000; color: #FFFFFF; font-size: 12px;");
    saveAsNewFileButton->setFixedHeight(30);

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

    QLabel *autoSaveLabel = new QLabel("Auto Save");
    autoSaveLabel->setStyleSheet("color: #FFFFFF; font-size: 12px;");
    autoSaveLabel->setAlignment(Qt::AlignCenter);

    QCheckBox *autoSaveCheckBox = new QCheckBox();
    autoSaveCheckBox->setFixedSize(20, 20);
    autoSaveCheckBox->setStyleSheet("color: #FFFFFF; font-size: 12px; border:2px solid #FFFFFF;");
    autoSaveCheckBox->setChecked(true);

    QHBoxLayout *autoSaveLayout = new QHBoxLayout();
    autoSaveLayout->setAlignment(Qt::AlignCenter);
    autoSaveLayout->addWidget(autoSaveCheckBox);

    menuLayout->addWidget(openButton);
    menuLayout->addWidget(saveButton);
    menuLayout->addWidget(saveAsNewFileButton);
    menuLayout->addWidget(fontSizeLabel);
    menuLayout->addWidget(fontSizeWidget);
    menuLayout->addWidget(autoSaveLabel);
    menuLayout->addLayout(autoSaveLayout);


    QTextEdit *textEdit = new QTextEdit;
    mainLayout->addWidget(textEdit);

    loadFileFromArguments(argc, argv, textEdit);

    QFont initialFont = textEdit->font();
    initialFont.setPointSize(12);
    textEdit->setFont(initialFont);
    textEdit->setStyleSheet("QTextEdit { line-height: 1.5; }");

    // Connect

    QObject::connect(openButton, &QPushButton::clicked, [&]() {
        openFile(textEdit);
        toggleMenu(menu);
    });

    QObject::connect(saveButton, &QPushButton::clicked, [&]() {
        saveFile(textEdit);
        toggleMenu(menu);
    });

    QObject::connect(saveAsNewFileButton, &QPushButton::clicked, [&](){
       saveAsNewFile(textEdit);
       toggleMenu((menu));
    });

    QObject::connect(button, &QPushButton::clicked, [&]() {
        toggleMenu(menu);
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

    QObject::connect(textEdit, &QTextEdit::textChanged, [&]() {
        isFileModified = true;
        updateFileNameLabel();
    });

    QObject::connect(textEdit, &QTextEdit::textChanged, [&]() {
        isFileModified = true;
        updateFileNameLabel();
        autoSaveFile(textEdit, autoSaveCheckBox);
    });


    // Shortcuts

    QShortcut *openShortcut = new QShortcut(QKeySequence("Ctrl+O"), &window);
    QObject::connect(openShortcut, &QShortcut::activated, [&]() {
        openFile(textEdit);
    });

    QShortcut *saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), &window);
    QObject::connect(saveShortcut, &QShortcut::activated, [&]() {
        saveFile(textEdit);
    });

    QShortcut *saveAsShortcut = new QShortcut(QKeySequence("Ctrl+Shift+S"), &window);
    QObject::connect(saveAsShortcut, &QShortcut::activated, [&]() {
        saveAsNewFile(textEdit);
    });

    // Start

    window.setLayout(mainLayout);
    window.show();

    return app.exec();
}
