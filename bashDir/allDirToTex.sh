#!/bin/bash

if [[ $# -ne 2 ]]
then
    echo "Usage: bash allDirToTex.sh <inDir> <doClean>"
    exit 1
fi

if [[ -d $1 ]]
then
    dummyVal=0
else
    echo "$1 is not a directory. exit 1"
    exit 1
fi

if [[ $2 == "0" ]]
then
    dummyVal=0
elif [[ $2 == "1" ]]
then
    dummyVal=0
else
    echo "$2 is not 0 or 1. exit 1"
    exit 1
fi

file=$1
for j in `seq 0 19`
do
    file=${file%//}
done
file=${file%/}
#
for j in `seq 0 19`
do
    file=${file#*/}
done

file=$file.tex
echo $file

echo "\\RequirePackage{xspace}" > $file
echo "\\RequirePackage{amsmath}" >> $file

echo "" >> $file

echo "\\documentclass[xcolor=dvipsnames]{beamer}" >> $file
echo "\\usetheme{Warsaw}" >> $file
echo "\\setbeamercolor{structure}{fg=NavyBlue!90!NavyBlue}" >> $file
echo "\\setbeamercolor{footlinecolor}{fg=white,bg=lightgray}" >> $file

echo "\\newcommand{\\pt}{\\ensuremath{p_{\\mathrm{T}}}\\xspace}" >> $file

echo "\\setbeamersize{text margin left=3pt,text margin right=3pt}" >> $file

echo "" >> $file

echo "\\setbeamertemplate{frametitle}" >> $file
echo "{" >> $file
echo "    \\nointerlineskip" >> $file
echo "    \\begin{beamercolorbox}[sep=0.1cm, ht=1.0em, wd=\\paperwidth]{frametitle}" >> $file
echo "        \\vbox{}\\vskip-2ex%" >> $file
echo "        \\strut\\insertframetitle\\strut" >> $file
echo "        \\vskip-0.8ex%" >> $file
echo "    \\end{beamercolorbox}" >> $file
echo "}" >> $file

echo "" >> $file

echo "\\setbeamertemplate{footline}{%" >> $file
echo "  \\begin{beamercolorbox}[sep=.4em,wd=\\paperwidth,leftskip=0.5cm,rightskip=0.5cm]{footlinecolor}" >> $file
echo "    \\hspace{0.15cm}%" >> $file
echo "    \\hfill\\insertauthor \\hfill\\insertpagenumber" >> $file
echo "  \\end{beamercolorbox}%" >> $file
echo "}" >> $file
echo "\\setbeamertemplate{navigation symbols}{}" >> $file

echo "" >> $file

echo "\\setbeamertemplate{itemize item}[circle]" >> $file
echo "\\setbeamertemplate{itemize subitem}[circle]" >> $file
echo "\\setbeamertemplate{itemize subsubitem}[circle]" >> $file
echo "\\setbeamercolor{itemize item}{fg=black}" >> $file
echo "\\setbeamercolor{itemize subitem}{fg=black}" >> $file
echo "\\setbeamercolor{itemize subsubitem}{fg=black}" >> $file

echo ""	>> $file

echo "\\definecolor{links}{HTML}{00BFFF}" >> $file
echo "\\hypersetup{colorlinks,linkcolor=links,urlcolor=links}" >> $file

echo "" >> $file

echo "\\author[CM]{YOURNAME}" >> $file

echo "" >> $file

echo "\\begin{document}" >> $file
echo "\\begin{frame}" >> $file
echo "\\setbeamercolor{coloredboxstuff}{fg=white,bg=NavyBlue!90!NavyBlue}" >> $file
echo "\\begin{beamercolorbox}[wd=1.1\\textwidth,sep=0.1em]{coloredboxstuff}" >> $file
echo "\\begin{center}" >> $file
echo "\\fontsize{15}{15}\\selectfont" >> $file
echo "Placeholder" >> $file
echo "\\end{center}" >> $file
echo "\\end{beamercolorbox}" >> $file
echo "\\end{frame}" >> $file

echo "" >> $file

slideWidth=362
slideHeight=225

for j in $1/*
do
    if [[ -f $j ]]
    then
	dummyVal=0
    else
	continue
    fi

    val=$(echo $j | grep -o "\." | wc -l)

    if [[ $val -ge 2 ]]
    then
	continue
    fi
    
    width=0
    height=0
    
    if [[ $j == *"pdf"* ]]
    then
	dummyVal=0
	convert $j -trim +repage tempAllDir.png 
    elif [[ $j == *"png"* ]]
    then
	cp $j tempAllDir.png
	dummyVal=0
    else
	echo "Warning: Skipping $j"
	continue
    fi
    

    width=$(file tempAllDir.png)
    rm tempAllDir.png
    
    width=${width#*PNG image data, }
    for k in `seq 0 19`
    do
	width=${width%","*}
    done     
    
    height=${width#*x }
    width=${width% x*}
        
    width=${width%.*}
    height=${height%.*}

#    echo $width, $height
#    continue

    startVal=100

    calcNum=$((startVal*$slideWidth*$height))
    calcDenom=$((width*100))
    
    while [[ $((calcNum/$calcDenom)) -gt $slideHeight ]]
    do	
	startVal=$((startVal - 5))

	if [[ $startVal -eq 10 ]]
	then
	    break
	fi

	calcNum=$((startVal*$slideWidth*$height))
	calcDenom=$((width*100))
    done

    fileName=$j

    while [[ $fileName == *"/"* ]]
    do
	fileName=${fileName#*/}
    done

    fileName=$(echo "$fileName" | sed 's/_//g')
    
    echo "$j"
    echo "$startVal $width $slideHeight = $calcNum"
    echo "100 $slideWidth $height = $calcDenom"
    echo "$calcNum/$calcDenom"
    echo ""
    
    echo "\\begin{frame}" >> $file
    echo "\\frametitle{\\centerline{Placeholder}}" >> $file
    if [[ startVal -eq 100 ]]
    then
	echo "\\includegraphics[width=1\\textwidth]{$j}" >> $file
    else
	echo "\\includegraphics[width=.$startVal\\textwidth]{$j}" >> $file
    fi
    echo "\\begin{itemize}" >> $file
    echo "\\fontsize{5}{5}\\selectfont" >> $file
#    echo "\\item{'$j'}" >> $file
    echo "\\item{$fileName}" >> $file
    echo "\\end{itemize}" >> $file
    echo "\\end{frame}" >> $file
    
    echo "" >> $file

    pos=$((pos + 1))
done

echo "\\begin{frame}" >> $file
echo "\\frametitle{\\centerline{Conclusions}}" >> $file
echo " \\begin{itemize}" >> $file
echo "  \\fontsize{10}{10}\\selectfont" >> $file
echo "  \\item{Placeholder}" >> $file
echo "  \\begin{itemize}" >> $file
echo "   \\fontsize{10}{10}\\selectfont" >> $file
echo "   \\item{Placeholder}" >> $file
echo "  \\end{itemize}" >> $file
echo " \\end{itemize}" >> $file
echo "\\end{frame}" >> $file


echo "" >> $file

echo "\\begin{frame}" >> $file
echo "\\setbeamercolor{coloredboxstuff}{fg=white,bg=NavyBlue!90!NavyBlue}" >> $file
echo "\\begin{beamercolorbox}[wd=1.1\\textwidth,sep=1.5em]{coloredboxstuff}" >> $file
echo "\\begin{center}" >> $file
echo "\\fontsize{15}{15}\\selectfont" >> $file
echo "Backup" >> $file
echo "\\end{center}" >> $file
echo "\\end{beamercolorbox}" >> $file
echo "\\end{frame}" >> $file

echo "" >> $file

echo "\\end{document}" >> $file

if [[ $2 -eq 1 ]]
then
    echo "Building..."
    fileName=${1%/}
    while [[ $fileName == *"/"* ]]
    do
	fileName=${fileName#*/}
    done
    
    pdflatex $fileName.tex
    rm $fileName.aux
    rm $fileName.log
    rm $fileName.nav
    rm $fileName.out
    rm $fileName.snm
    rm $fileName.toc
    rm -f *.*~
    echo "Cleaning..."    
fi

echo "allDirToTex.sh Complete!"
