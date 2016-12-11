declare variable $url as xs:string external;
for $entry in doc($url)/GameSaveManager_DatabaseUpdate/entry
return element entry {
        attribute name {$entry/@id},
        attribute title {$entry/title/text()},
        for $loc in $entry/directories/dir
        return element location {
                attribute type {if ($loc/path/@specialpath = '%REGISTRY%') then 'registry' else 'path'},
                attribute ev {translate($loc/path/@specialpath, '%', '')},
                attribute path {$loc/path/text()},
                attribute root {$loc/reg/hive/text()},
                attribute key {$loc/reg/path/text()},
                attribute value {$loc/reg/value/text()},
                element file {attribute includepath {$loc/include/text()}},
                element file {attribute excludepath {$loc/exclude/text()}}}}
