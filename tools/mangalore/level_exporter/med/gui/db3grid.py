#-------------------------------------------------------------------------------
# db3grid.py
#-------------------------------------------------------------------------------

import wx
import wx.grid as grid

class Db3Grid(grid.Grid):
    def __init__(self, table, parent):
        grid.Grid.__init__(self, parent)
        self.table = table
        desc = parent.db3File.getTableDescription(table)
        content = parent.db3File.getTableContent(table)
        col = len(desc)
        row = len(content)
        self.CreateGrid(row, col)
        for i in range(col):
            self.SetColLabelValue(i, desc[i][1])
        for i in range(row):
            for j in range(col):
                value = content[i][j]
                if value:
                    self.SetCellValue(i, j, value)
        self.SetRowLabelSize(25)
        self.SetColLabelSize(25)
        self.AutoSizeColumns(False)
        self.CanDragColSize()
        self.selectedCells = []
        self.changedCells = []
        self.Bind(grid.EVT_GRID_CELL_CHANGE, self.onCellChanged)
        self.Bind(grid.EVT_GRID_CELL_RIGHT_CLICK, self.onCellRClick)

    def save(self):
        for i in self.changedCells:
            row = i.GetRow()
            col = i.GetCol()
            rowName = self.GetRowLabelValue(row)
            colName = self.GetColLabelValue(col)
            self.GetParent().db3File.setCellData(self.table, rowName, colName, self.GetCellValue(row, col))
            self.SetCellTextColour(row, col, wx.Color(0, 0, 0))
        self.GetParent().db3File.save()
        self.changedCells = []
        self.ForceRefresh()

# private
    def onCellChanged(self, event):
        row = event.GetRow()
        col = event.GetCol()
        self.GetParent().changed = True
        self.GetParent().GetParent().GetParent().GetParent().GetParent().enableSaveTool()
        coord = grid.GridCellCoords(row, col)
        if coord not in self.changedCells:
            self.changedCells.append(coord)
            self.SetCellTextColour(row, col, wx.Color(255, 0, 0))

    def onCellRClick(self, event):
        if not hasattr(self, 'menuId'):
            self.menuId = wx.NewId()
            self.Bind(wx.EVT_MENU, self.onPopupUpdate, id = self.menuId)
        menu = wx.Menu()
        self.getSelectedCells()
        if len(self.selectedCells):
            item = wx.MenuItem(menu, self.menuId, 'Update data in selected cells')
        else:
            self.selectedCells.append(grid.GridCellCoords(event.GetRow(), event.GetCol()))
            item = wx.MenuItem(menu, self.menuId, 'Update data in clicked cell')
        menu.AppendItem(item)
        self.PopupMenu(menu)
        menu.Destroy()

    def onPopupUpdate(self, event):
        for i in self.selectedCells:
            row = i.GetRow()
            col = i.GetCol()
            rowName = self.GetRowLabelValue(row)
            colName = self.GetColLabelValue(col)
            coord = grid.GridCellCoords(row, col)
            if coord in self.changedCells:
                self.GetParent().db3File.setCellData(self.table, rowName, colName, self.GetCellValue(row, col))
                self.GetParent().db3File.save()
                self.changedCells.remove(grid.GridCellCoords(row, col))
                self.SetCellTextColour(row, col, wx.Color(0, 0, 0))
        if not len(self.changedCells):
            self.GetParent().GetParent().GetParent().GetParent().GetParent().enableSaveTool(False)
        self.ForceRefresh()

    def getSelectedCells(self):
        self.selectedCells = []
        blockTl = self.GetSelectionBlockTopLeft()
        blockBr = self.GetSelectionBlockBottomRight()
        for i in range(len(blockTl)):
            for row in range(blockTl[i][0], blockBr[i][0] + 1):
                for col in range(blockTl[i][1], blockBr[i][1] + 1):
                    self.selectedCells.append(grid.GridCellCoords(row, col))
        for i in self.GetSelectedRows():
            for col in range(self.GetNumberCols()):
                self.selectedCells.append(grid.GridCellCoords(i, col))
        for i in self.GetSelectedCols():
            for row in range(self.GetNumberRows()):
                self.selectedCells.append(grid.GridCellCoords(row, i))
        for i in self.GetSelectedCells():
            self.selectedCells.append(grid.GridCellCoords(i[0], i[1]))

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------