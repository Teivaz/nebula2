-- -----------------------------------------------------------
-- $parser:nluaserver$ $class:nscriptablesector$
-- -----------------------------------------------------------
call('addvisibleobject', [[object1]], [[/usr/testobject]], 0.000000, 0.000000, 10.000000, 1.000000)
call('addvisibleobject', [[object2]], [[/usr/testobject]], 0.000000, 0.000000, 20.000000, 1.000000)
call('addvisibleobject', [[object3]], [[/usr/testobject]], 0.000000, 0.000000, -10.000000, 1.000000)
call('addvisibleobject', [[object4]], [[/usr/testobject]], 0.000000, 1.000000, -20.000000, 1.000000)
call('addvisibleobject', [[object5]], [[/usr/testobject]], 0.000000, 2.000000, -20.000000, 1.000000)
call('addoccludingobject', [[blocker1]], 0.000000, 0.000000, -6.000000, 1.000000, 1.000000, -5.000000)
call('addvisibleobject', [[blockervis1]], [[/usr/occluderobject]], 0.500000, 0.500000, -5.500000, 0.500000)
-- -----------------------------------------------------------
-- Eof
