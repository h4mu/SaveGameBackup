declare variable $os as xs:string external;
declare variable $url as xs:string external;
for $entry in doc($url)/programs/game
return element entry {
    $entry/@name,
    attribute title {$entry/title/text()},
    for $loc in $entry/version[@os=$os or @os='DOS']/locations/*[not(@name)]
    return element location {
        attribute type {local-name($loc)},
        $loc/@*,
        for $file in $loc/../../files//*[@filename or @path]
        return element file
            {attribute {concat(local-name($file), 'path')}
                {if ($file/@filename and $file/@path)
                then concat($file/@path, '/', $file/@filename)
                else $file/@*[1]}}}}
