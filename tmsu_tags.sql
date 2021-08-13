SELECT file.directory, file.name as filename, tag.name AS tag FROM 
file 
INNER JOIN file_tag ON file.id = file_tag.file_id
INNER JOIN tag ON file_tag.tag_id = tag.id
