Add-Type -AssemblyName System.Drawing

function Convert565ToColor([UInt16]$Value) {
    $r = ($Value -shr 11) -band 31
    $g = ($Value -shr 5) -band 63
    $b = $Value -band 31
    [System.Drawing.Color]::FromArgb(
        255,
        [int](($r * 255 + 15) / 31),
        [int](($g * 255 + 31) / 63),
        [int](($b * 255 + 15) / 31)
    )
}

function ConvertColorTo565([System.Drawing.Color]$Color) {
    $r = [int](($Color.R * 31 + 127) / 255)
    $g = [int](($Color.G * 63 + 127) / 255)
    $b = [int](($Color.B * 31 + 127) / 255)
    [UInt16](($r -shl 11) -bor ($g -shl 5) -bor $b)
}

function Read-SpkSprite($Reader) {
    $w = $Reader.ReadUInt16()
    $h = $Reader.ReadUInt16()
    $bmp = New-Object System.Drawing.Bitmap ([int]$w), ([int]$h), ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)

    for ($y = 0; $y -lt $h; $y++) {
        $len = $Reader.ReadUInt16()
        $words = New-Object UInt16[] $len
        for ($i = 0; $i -lt $len; $i++) {
            $words[$i] = $Reader.ReadUInt16()
        }

        if ($len -eq 0) {
            continue
        }

        $count = $words[0]
        $idx = 1
        $x = 0
        for ($segment = 0; $segment -lt $count; $segment++) {
            if ($idx + 1 -ge $words.Length) {
                break
            }
            $transparent = $words[$idx]
            $colorCount = $words[$idx + 1]
            $idx += 2
            $x += $transparent

            for ($c = 0; $c -lt $colorCount; $c++) {
                if ($idx -ge $words.Length -or $x -ge $w) {
                    break
                }
                $bmp.SetPixel($x, $y, (Convert565ToColor $words[$idx]))
                $idx++
                $x++
            }
        }
    }

    $bmp
}

function Read-Spk($Path) {
    $stream = [System.IO.File]::OpenRead($Path)
    try {
        $reader = New-Object System.IO.BinaryReader($stream)
        $count = $reader.ReadUInt16()
        $sprites = New-Object System.Collections.Generic.List[System.Drawing.Bitmap]
        for ($i = 0; $i -lt $count; $i++) {
            $sprites.Add((Read-SpkSprite $reader))
        }
        $sprites
    }
    finally {
        $stream.Close()
    }
}

function Write-SpkSprite($Writer, [System.Drawing.Bitmap]$Bitmap) {
    $Writer.Write([UInt16]$Bitmap.Width)
    $Writer.Write([UInt16]$Bitmap.Height)

    for ($y = 0; $y -lt $Bitmap.Height; $y++) {
        $line = New-Object System.Collections.Generic.List[UInt16]
        $line.Add([UInt16]0)
        $segments = 0
        $x = 0

        while ($x -lt $Bitmap.Width) {
            $transparent = 0
            while ($x -lt $Bitmap.Width -and $Bitmap.GetPixel($x, $y).A -eq 0) {
                $transparent++
                $x++
            }

            if ($x -ge $Bitmap.Width) {
                break
            }

            $colors = New-Object System.Collections.Generic.List[UInt16]
            while ($x -lt $Bitmap.Width) {
                $color = $Bitmap.GetPixel($x, $y)
                if ($color.A -eq 0) {
                    break
                }
                $colors.Add((ConvertColorTo565 $color))
                $x++
            }

            $line.Add([UInt16]$transparent)
            $line.Add([UInt16]$colors.Count)
            foreach ($word in $colors) {
                $line.Add($word)
            }
            $segments++
        }

        $line[0] = [UInt16]$segments
        $Writer.Write([UInt16]$line.Count)
        foreach ($word in $line) {
            $Writer.Write([UInt16]$word)
        }
    }
}

function Write-Spk($Path, [System.Collections.Generic.List[System.Drawing.Bitmap]]$Sprites) {
    $tmp = "$Path.tmp"
    $tmpIndex = "$Path" + "i.tmp"
    $stream = [System.IO.File]::Create($tmp)
    $indexStream = [System.IO.File]::Create($tmpIndex)
    try {
        $writer = New-Object System.IO.BinaryWriter($stream)
        $indexWriter = New-Object System.IO.BinaryWriter($indexStream)
        $writer.Write([UInt16]$Sprites.Count)
        $indexWriter.Write([UInt16]$Sprites.Count)

        $offsets = New-Object System.Collections.Generic.List[UInt32]
        foreach ($sprite in $Sprites) {
            $offsets.Add([UInt32]$stream.Position)
            Write-SpkSprite $writer $sprite
        }

        foreach ($offset in $offsets) {
            $indexWriter.Write([UInt32]$offset)
        }
    }
    finally {
        $stream.Close()
        $indexStream.Close()
    }

    Move-Item -LiteralPath $tmp -Destination $Path -Force
    Move-Item -LiteralPath $tmpIndex -Destination ($Path + "i") -Force
}

function Export-SpkSprites($Path, [int[]]$Ids, $Prefix) {
    $sprites = Read-Spk $Path
    try {
        foreach ($id in $Ids) {
            $out = "C:\newdk\${Prefix}_${id}.png"
            $sprites[$id].Save($out, [System.Drawing.Imaging.ImageFormat]::Png)
            Write-Output "$out $($sprites[$id].Width)x$($sprites[$id].Height)"
        }
    }
    finally {
        foreach ($sprite in $sprites) {
            $sprite.Dispose()
        }
    }
}
