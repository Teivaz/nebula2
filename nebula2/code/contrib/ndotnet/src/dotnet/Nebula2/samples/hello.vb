Imports N2 = Nebula2NET

Module Hello

    Sub Main()
        ' Note: a NOH name can only be set via ctor!
        ' Note: The ctor takes a NOH name as a parameter.
        ' You can use either a full path, starting with '/'
        ' or a relative path, relative to nebula's cwd!
        ' Note: The ctor only creates a new nebula(c++) object
        ' if it does not already exist in the nebula NOH, otherwise
        ' the ctor returns the existing nebula(c++) object!
        ' Note: The ctor throws an exception if the type of the 
        ' nebula(vb.net) object does not match the type of 
        ' the existing nebula(c++) object!
        Dim foo As New N2.nroot("/")
        System.Console.Out.WriteLine(foo.getclass())

        Dim timeserver As New N2.ntimeserver("/sys/servers/time")
        System.Console.Out.WriteLine(timeserver.gettime())

        'create some nroot objects
        Dim bar As New N2.nroot("/hallo/spencer")
        bar = New N2.nroot("/hallo/lory")
        bar = New N2.nroot("hallo")

        ' This is really an array of nroot objects
        Dim children() = bar.getchildren()

        If children.Length() > 0 Then
            System.Console.Out.WriteLine("Children:")
        End If

        For Each child As N2.nroot In children
            System.Console.Out.WriteLine(child.getname())
        Next

        If foo.isa("nroot") = True Then
            System.Console.Out.WriteLine("foo is an nroot!")
        End If

        If foo.isa("ntimeserver") <> True Then
            System.Console.Out.WriteLine("foo is not a timeserver!")
        End If

    End Sub

End Module
