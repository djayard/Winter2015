function findBestPlacement(ColorIndex, XTile, YTile)
    Best = {
	X = -1,	
	Y = -1,
	Rotation = 0,
	BCoverage = 0,
	BInterference = 0
    }
    for RotationIndex = 0, 3, 1 do
        for YOffset = -2-test.getWallShapeHeight(ColorIndex), 4, 1 do
            TargetY = YTile + YOffset
            if 0 > TargetY then
                goto ycontinue
            end
            if TargetY >= test.DMapHeight_rebuild then
                goto ycontinue
            end
            for XOffset = -2-test.getWallShapeWidth(ColorIndex), 4, 1 do
                TargetX = XTile + XOffset
                if 0 > TargetX then
                    goto xcontinue
                end
                if TargetX >= test.DMapWidth_rebuild then
                    goto xcontinue
                end
                if test.getValidWallPlacement(ColorIndex, TargetX, TargetY) then    
                    Coverage = 0
                    Interference = 0
                    BetterSolution = false
                    for WallYPos = 0, test.getWallShapeHeight(ColorIndex)-1, 1 do
                        for WallXPos = 0, test.getWallShapeWidth(ColorIndex)-1, 1 do
	                    --print("2", WallYPos, WallXPos, ColorIndex)
	                    if test.getWallShapeIsBlock(ColorIndex, WallXPos, WallYPos) then
		                --print("3", WallYPos, WallXPos, ColorIndex)
	                        WallX = TargetX + WallXPos - XTile
		                WallY = TargetY + WallYPos - YTile
		                if ((WallX == -3 or WallX == 4) and (WallY >= -2 and WallY <= 3)) then
                                    Coverage = Coverage+1
		                elseif ((WallY == -3 or WallY == 4) and (WallX >= -3 and WallX <= 4)) then
                                    Coverage = Coverage+1
		                elseif ((WallX >= -2 and WallX <= 3) and (WallY >= -2 and WallY <= 3)) then
                                    Interference = Interference+1
		                end
                            end
	                    --print("4", WallYPos,WallXPos, ColorIndex)     
                        end    
                    end
                    if Best.X < 0 then
                        BetterSolution = true
	                
                    end
                    if Coverage > Best.BCoverage then
                        BetterSolution = true
	                
                    end
                    if Coverage == Best.BCoverage and Interference < Best.BInterference then
                        BetterSolution = true
	              
                    end
                    if BetterSolution then
                        
                        Best.X = TargetX
                        Best.Y = TargetY
                        Best.Rotation = RotationIndex
                        Best.BCoverage = Coverage
                        Best.BInterference = Interference
                    end
                --print ("lua: ", Best.X, Best.Y, Best.Rotation, Best.BCoverage, Best.BInterference)
                end
            ::xcontinue::
            end
        ::ycontinue::
        end
    test.rotateWallShape(ColorIndex)
    end
    return Best
end

