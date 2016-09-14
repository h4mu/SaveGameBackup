for $loc in doc('http://gamesave.info/api/GameSaveInfo202/')/programs/game/version[@os='Windows' or @os='DOS']/locations/*[not(@name)]
return element location {$loc/../../../@name,
        attribute title {$loc/../../../title/text()},
        attribute type {local-name($loc)},
        $loc/@*,
        for $file in $loc/../../files//*[@filename or @path]
        return element file
            {attribute {concat(local-name($file), 'path')}
                {if ($file/@filename and $file/@path)
                then concat($file/@path, '\', $file/@filename)
                else $file/@*[1]}}}
