SELECT session_id, mouse_number, training_set_name, session_status, session_date,  Rw_LicksPerMin(session_id) as rw_licks_per_min, rw_licks FROM maze.mouses_sessions as ms
INNER JOIN mouse_batches as mb
ON ms.mouse_batch = mb.id_batches
INNER JOIN mouses as mo
ON ms.mouses_id = mo.mouses_id
INNER JOIN training_set_def as ts
ON ts.training_set_id = ms.training_set_id
WHERE batch_name = 'MORJ_controls_2' and training_set_name in ('Openfield_joint_active','Openfield_joint_inactive') 
order by mouse_number, session_date;