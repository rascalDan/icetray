-- libdbpp-postgresql
-- libdbpp:no-cursor
-- libdbpp:page-size:12
SELECT COUNT(*)
FROM testTable
WHERE id = ?
AND name = ?
