 QT		+= core gui widgets network
 TARGET	= vactracker
 HEADERS= parse.h user.h deposit.h withdrawal.h link.h mainWindow.h qtreespecialitem.h vacEdit.h userEdit.h regionEdit.h region.h regionYear.h discover.h pwdChange.h addDeposit.h
 HEADERS+= depositparser.h withdrawalparser.h userparser.h regionparser.h regionyearparser.h
 SOURCES= main.cpp parse.cpp link.cpp mainWindow.cpp vacEdit.cpp userEdit.cpp regionEdit.cpp discover.cpp pwdChange.cpp addDeposit.cpp
 SOURCES+= depositparser.cpp withdrawalparser.cpp userparser.cpp regionparser.cpp regionyearparser.cpp
 RC_FILE= vactracker.rc

include($$PWD/qtzeroconf/qtzeroconf.pri)

                 
 

