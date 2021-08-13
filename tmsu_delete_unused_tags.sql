DELETE FROM tag WHERE id NOT IN (SELECT tag_id from file_tag) 
