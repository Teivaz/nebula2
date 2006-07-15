#-------------------------------------------------------------------------------
# db3file.py
#-------------------------------------------------------------------------------

import os, shutil
from pysqlite2 import dbapi2 as sqlite

class Db3File:
    def __init__(self):
        self.pathName = ''
        self.connection = None
        self.cursor = None

    def open(self, pathName):
        self.pathName = pathName
        self.connection = sqlite.connect(pathName)
        self.cursor = self.connection.cursor()

    def save(self):
        self.connection.commit()

    def saveAs(self, pathName):
        root, ext = os.path.splitext(pathName)
        tempName = root + '.tmp'
        shutil.copyfile(self.pathName, tempName)
        self.save()
        self.close()
        shutil.move(self.pathName, pathName)
        shutil.move(tempName, self.pathName)
        self.open(pathName)

    def close(self):
        self.connection.close()

    def listTables(self):
        sql = "SELECT name FROM sqlite_master WHERE type='table'"
        self.cursor.execute(sql)
        return [i[0] for i in self.cursor.fetchall()]

    def listIndecies(self):
        sql = "SELECT name FROM sqlite_master WHERE type='index'"
        self.cursor.execute(sql)
        return [i[0] for i in self.cursor.fetchall()]

    def getTableDescription(self, table):
        sql = "PRAGMA TABLE_INFO(%s)"
        self.cursor.execute(sql % table)
        return [i[:3] for i in self.cursor.fetchall()]

    def getTableContent(self, table):
        sql = "SELECT * FROM %(table)s ORDER BY rowid"
        self.cursor.execute(sql % {'table' : table})
        return self.cursor.fetchall()

    def setCellData(self, table, row, col, data):
        sql = "UPDATE %(table)s SET %(col)s='%(data)s' WHERE rowid=%(row)s"
        self.cursor.execute(sql % {'table' : table, 'col' : col, 'data' : data, 'row' : row})

    def createEmpty(self):
        sql = "DROP TABLE %s"
        for i in self.listTables():
            self.cursor.execute(sql % i)
        sql = "DROP INDEX %s"
        for i in self.listIndecies():
            self.cursor.execute(sql % i)
        sql = "CREATE TABLE _Entities ('_Type' TEXT, 'GUID' TEXT, '_ID' TEXT, '_Category' TEXT, '_Level' TEXT, \
              'Transform' TEXT, 'LightType' TEXT, 'LightColor' TEXT, 'LightRange' TEXT, 'LightAmbient' TEXT, \
              'LightCastShadows' TEXT, 'AnimPath' TEXT, 'Graphics' TEXT, 'Physics' TEXT, 'Center' TEXT, \
              'Extents' TEXT, 'StartLevel' TEXT, 'Id' TEXT, 'Placeholder' TEXT, 'LE' TEXT, 'AE' TEXT, 'AT' TEXT, \
              'PA' TEXT, 'Name' TEXT, 'AnimSet' TEXT, 'InputFocus' TEXT, 'CameraFocus' TEXT, 'MaxVelocity' TEXT, \
              'FieldOfView' TEXT, 'Time' TEXT)"
        self.cursor.execute(sql)
        sql = "CREATE TABLE _Categories ('Name' TEXT)"
        self.cursor.execute(sql)
        sql = "CREATE TABLE _Navigation ('GUID' TEXT, '_Level' TEXT, 'Transform' TEXT, 'File' TEXT)"
        self.cursor.execute(sql)
        sql = "CREATE TABLE _Global ('_Dummy' TEXT, 'CurrentLevel' TEXT)"
        self.cursor.execute(sql)
        sql = "CREATE INDEX _Entities_Index ON _Entities ('_Type', 'GUID', '_ID', '_Category', '_Level')"
        self.cursor.execute(sql)

    def queryLevelIds(self):
        sql = "SELECT Id FROM _Entities WHERE _Type='TEMPLATE' AND _Category='.Levels'"
        self.cursor.execute(sql)
        return [i[0] for i in self.cursor.fetchall()]

    def hasLevelId(self, id):
        sql = "SELECT Id FROM _Entities WHERE _Type='TEMPLATE' AND _Category='.Levels' AND Id='%s'"
        self.cursor.execute(sql % id)
        return len(self.cursor.fetchall())

    def queryLevelData(self, id, type):
        sql = "SELECT %(type)s FROM _Entities WHERE _Type='TEMPLATE' AND _Category='.Levels' AND Id='%(id)s'"
        self.cursor.execute(sql % {'type' : type, 'id' : id})
        return self.cursor.fetchall()[0][0]

    def updateLevelData(self, id, type, data):
        if not self.hasLevelId(id):
            sql = "INSERT INTO _Entities (_Type, _Category, Id) VALUES('TEMPLATE', '.Levels', '%s')"
            self.cursor.execute(sql % id)
        sql = "UPDATE _Entities SET %(type)s='%(data)s' WHERE _Type='TEMPLATE' AND _Category='.Levels' AND Id='%(id)s'"
        self.cursor.execute(sql % {'type' : type, 'data' : data, 'id' : id})

    def queryCategoryNames(self):
        sql = "SELECT Name FROM _Categories"
        self.cursor.execute(sql)
        return [i[0] for i in self.cursor.fetchall()]

    def hasCategoryName(self, name):
        sql = "SELECT Name FROM _Categories WHERE Name='%s'"
        self.cursor.execute(sql % name)
        return len(self.cursor.fetchall())

    def updateCategoryName(self, name):
        if not self.hasCategoryName(name):
            sql = "INSERT INTO _Categories (Name) VALUES('%s')"
            self.cursor.execute(sql % name)

    def queryGlobalDummy(self, type):
        sql = "SELECT _Dummy FROM _Global WHERE '%s'!=''"
        self.cursor.execute(sql % type)
        return self.cursor.fetchall()[0][0]

    def queryGlobalValue(self, type):
        sql = "SELECT %(type)s FROM _Global WHERE '%(type)s'!=''"
        self.cursor.execute(sql % {'type' : type})
        return self.cursor.fetchall()[0][0]

    def hasGlobalValue(self, type):
        sql = "SELECT _Dummy FROM _Global WHERE '%s'!=''"
        self.cursor.execute(sql % type)
        return len(self.cursor.fetchall())

    def addGlobalValue(self, type, dummy, value):
        sql = "INSERT INTO _Global (_Dummy, %(type)s) VALUES('%(dummy)s', '%(value)s')"
        self.cursor.execute(sql % {'type' : type, 'dummy' : dummy, 'value' : value})

    def updateGlobalValue(self, type, value):
        dummy = self.queryGlobalDummy(type)
        sql = "UPDATE _Global SET %(type)s='%(value)s' WHERE _Dummy='%(dummy)s'"
        self.cursor.execute(sql % {'type' : type, 'value' : value, 'dummy' : dummy})

    def queryEntityGuid(self, category, level):
        sql = "SELECT GUID FROM _Entities WHERE _Type='INSTANCE' AND _Category='%(category)s' AND _Level='%(level)s'"
        self.cursor.execute(sql % {'category' : category, 'level' : level})
        return [i[0] for i in self.cursor.fetchall()]

    def hasEntityCategoryId(self, category, id):
        sql = "SELECT _Category FROM _Entities WHERE _Type='TEMPLATE' AND _Category='%(category)s' AND Id='%(id)s'"
        self.cursor.execute(sql % {'category' : category, 'id' : id})
        return len(self.cursor.fetchall())

    def hasEntityGuid(self, guid):
        sql = "SELECT GUID FROM _Entities WHERE _Type='INSTANCE' AND GUID='%s'"
        self.cursor.execute(sql % guid)
        return len(self.cursor.fetchall())

    def queryEntityData(self, guid, type):
        sql = "SELECT %(type)s FROM _Entities WHERE _Type='INSTANCE' AND GUID='%(guid)s'"
        self.cursor.execute(sql % {'type' : type, 'guid' : guid})
        return self.cursor.fetchall()[0][0]

    def updateEntityCategory(self, category, id, type, data):
        if not self.hasEntityCategoryId(category, id):
            sql = "INSERT INTO _Entities (_Type, _Category, Id) VALUES('TEMPLATE', '%(category)s', '%(id)s')"
            self.cursor.execute(sql % {'category' : category, 'id' : id})
        sql = "UPDATE _Entities SET %(type)s='%(data)s' WHERE _Type='TEMPLATE' AND _Category='%(category)s' AND Id='%(id)s'"
        self.cursor.execute(sql % {'type' : type, 'data' : data, 'category' : category, 'id' : id})

    def updateEntityData(self, guid, type, data):
        if not self.hasEntityGuid(guid):
            sql = "INSERT INTO _Entities (_Type, GUID) VALUES('INSTANCE', '%s')"
            self.cursor.execute(sql % guid)
        sql = "UPDATE _Entities SET %(type)s='%(data)s' WHERE _Type='INSTANCE' AND GUID='%(guid)s'"
        self.cursor.execute(sql % {'type' : type, 'data' : data, 'guid' : guid})

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------