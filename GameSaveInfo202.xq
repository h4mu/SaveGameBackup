declare variable $os as xs:string external;
declare variable $batch as xs:string external;
for $loc in doc(concat('http://rawcdn.githack.com/h4mu/Data/master/', $batch, '.xml'))/programs/game/version[@os=$os or @os='DOS']/locations/*[not(@name)]
return element location {$loc/../../../@name,
        attribute title {$loc/../../../title/text()},
        attribute type {local-name($loc)},
        $loc/@*,
        for $file in $loc/../../files//*[@filename or @path]
        return element file
            {attribute {concat(local-name($file), 'path')}
                {if ($file/@filename and $file/@path)
                then concat($file/@path, '/', $file/@filename)
                else $file/@*[1]}}}
