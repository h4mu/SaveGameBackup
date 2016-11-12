declare variable $url as xs:string external;
for $loc in doc($url)/GameSaveManager_DatabaseUpdate/entry/directories/dir
return element location {$loc/../../@id,
        attribute title {$loc/../../title/text()},
        attribute type {if ($loc/path/@specialpath = '%REGISTRY%') then 'registry' else 'path'},
        attribute ev {translate($loc/path/@specialpath, '%', '')},
        attribute path {$loc/path/text()},
        attribute root {$loc/reg/hive/text()},
        attribute key {$loc/reg/path/text()},
        attribute value {$loc/reg/value/text()},
        element file {attribute includepath {$loc/include/text()}},
        element file {attribute excludepath {$loc/exclude/text()}}}
