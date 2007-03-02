-------------------------------------------------------------------------------
--  physxxmlutil.ms
--
--  (c)2007 Cho, Jun Hyeong
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Simple XML library for 3dsmax script
--
-------------------------------------------------------------------------------
struct xmlElementNode ( name, hasChild )
global xmlOutputFile;
global xmlElementStack = #();           -- 현재 계층 구조의 엘리먼트 Stack

-------------------------------------------------------------------------------
-- @name        xmlTabString
-- @param       count : tab count
-- @return      count 만큼의 tab string 을 리턴한다.
-------------------------------------------------------------------------------
fn xmlTabString count =
(
    local tabString = ""

    for i = 1 to count do
    (
        append tabString "\t"
    )

    return tabString;
)

-------------------------------------------------------------------------------
-- @name        xmlBegin
-- @param       outfile
-- @desc
-------------------------------------------------------------------------------
fn xmlBegin outfile =
(
    xmlOutputFile = outfile;
)

-------------------------------------------------------------------------------
-- @name        xmlEnd
-- @param
-- @desc
-------------------------------------------------------------------------------
fn xmlEnd =
(
    xmlOutputFile = undefined;
    -- empty
)

-------------------------------------------------------------------------------
-- @name        xmlElementBegin
-- @param       name : element name
-- @desc
-------------------------------------------------------------------------------
fn xmlElementBegin name =
(
    count = xmlElementStack.count;

    if( count > 0 ) then
    (
        format ">\n" to:xmlOutputFile
        xmlElementStack[count].hasChild = true;
        tabString = xmlTabString(count)
        format "%" tabString to:xmlOutputFile;
    )

    format "<%s" name to:xmlOutputFile;

    newElement = xmlElementNode name false
    append xmlElementStack newElement;
)

-------------------------------------------------------------------------------
-- @name        xmlElementEnd
-- @param
-- @desc
-------------------------------------------------------------------------------
fn xmlElementEnd =
(
    count = xmlElementStack.count;
    currentElement = xmlElementStack[count];

    if( currentElement.hasChild == true ) then
    (
        tabString = xmlTabString(count-1)
        format "\n%" tabString to:xmlOutputFile;
        format "</%>" currentElement.name to:xmlOutputFile;
    )
    else
    (
        format "/>" to:xmlOutputFile
    )

    deleteItem xmlElementStack count;
)

-------------------------------------------------------------------------------
-- @name        xmlAddAttribute
-- @param       name : attribute name
--              value : attribute value
-- @desc
-------------------------------------------------------------------------------
fn xmlAddAttribute name value =
(
    format " %=\"%\"" name value to:xmlOutputFile
)

-------------------------------------------------------------------------------
-- @name        test
--
-- @desc    test function
-------------------------------------------------------------------------------
fn test =
(
    outfile = createFile "d:/a.xml"
    xmlBegin outfile
    xmlElementBegin "test"
    xmlAddAttribute "aaa" "A"
    xmlElementBegin "test2"
    xmlElementEnd()
    xmlElementEnd()

)

-------------------------------------------------------------------------------
-- EOF
-------------------------------------------------------------------------------
