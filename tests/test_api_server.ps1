# SafeSignal API Server Test Script
Write-Host "=========================================="
Write-Host "   TESTING SAFESIGNAL REST API SERVER     "
Write-Host "=========================================="

$serverProcess = Start-Process -FilePath ".\SafeSignalServer.exe" -PassThru
Start-Sleep -Seconds 1

$allPassed = $true
function Assert-Test($desc, $cond) {
    if ($cond) {
        Write-Host " [PASS] $desc" -ForegroundColor Green
    } else {
        Write-Host " [FAIL] $desc" -ForegroundColor Red
        $global:allPassed = $false
    }
}

try {
    # 1. Health
    $h = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/health" -Method Get
    Assert-Test "Health endpoint status is ok" ($h.status -eq "ok")

    # 2. Status initial
    $s = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/status" -Method Get
    Assert-Test "Initial risk score is 0" ($s.riskScore -eq 0)
    Assert-Test "Initial risk level is Normal" ($s.riskLevel -eq "Normal")

    # 3. Add Contact
    $cBody = @{ id = 101; name = "Sarah Connor"; phone = "555-0199"; relationship = "Mother"; priority = 1 } | ConvertTo-Json
    $cRes = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/contacts" -Method Post -Body $cBody -ContentType "application/json"
    Assert-Test "Register contact succeeded" ($cRes.success -eq $true)

    # 4. Get Contacts
    $contacts = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/contacts" -Method Get
    Assert-Test "Contacts count is 1" ($contacts.count -ge 1)
    Assert-Test "Contact name matches" ($contacts.contacts[0].name -eq "Sarah Connor")

    # 5. Start Journey
    $jBody = @{
        startLocation = "Library";
        destination = "Dorm C";
        eta = "19:00:00";
        route = @("Main Gate", "Campus Quad", "Dining Hall")
    } | ConvertTo-Json
    $jRes = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/journey/start" -Method Post -Body $jBody -ContentType "application/json"
    Assert-Test "Start journey succeeded" ($jRes.success -eq $true)

    # 6. Get Journey
    $j = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/journey" -Method Get
    Assert-Test "Journey is active" ($j.active -eq $true)
    Assert-Test "Next expected checkpoint is Main Gate" ($j.nextExpectedCheckpoint -eq "Main Gate")

    # 7. Update Journey Location
    $locBody = @{ location = "Main Gate"; timestamp = "18:15:00" } | ConvertTo-Json
    $lRes = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/journey/location" -Method Post -Body $locBody -ContentType "application/json"
    Assert-Test "Location update succeeded" ($lRes.success -eq $true)

    $jAfter = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/journey" -Method Get
    Assert-Test "Advanced to next checkpoint" ($jAfter.currentCheckpointIndex -eq 1)

    # 8. Schedule Check-in
    $ciBody = @{ time = "18:30:00"; description = "Midpoint walk"; location = "Campus Quad" } | ConvertTo-Json
    $ciRes = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/checkins/schedule" -Method Post -Body $ciBody -ContentType "application/json"
    Assert-Test "Schedule checkin succeeded" ($ciRes.success -eq $true)
    $cid = $ciRes.checkInID

    # 9. Get Checkins
    $cis = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/checkins" -Method Get
    Assert-Test "Check-in list contains scheduled item" ($cis.total -ge 1)

    # 10. Confirm Check-in
    $confBody = @{ checkInID = $cid } | ConvertTo-Json
    $confRes = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/checkins/confirm" -Method Post -Body $confBody -ContentType "application/json"
    Assert-Test "Confirm checkin succeeded" ($confRes.success -eq $true)

    # 11. Ingest Simulated Sensor Event (Fall Detected)
    $evBody = @{ type = "Fall Detected"; location = "Campus Quad"; timestamp = "18:32:00" } | ConvertTo-Json
    $evRes = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/sim/event" -Method Post -Body $evBody -ContentType "application/json"
    Assert-Test "Simulated event ingestion succeeded" ($evRes.success -eq $true)
    Assert-Test "Risk score increased after fall" ($evRes.newRiskScore -gt 0)

    # 12. Check Verifications
    $verifs = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/verifications" -Method Get
    Assert-Test "Verification state query succeeded" ($null -ne $verifs.hasActive)

    # 13. Confirm User Safe
    if ($verifs.hasActive) {
        $csRes = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/emergency/verify/confirm-safe" -Method Post -Body "{}" -ContentType "application/json"
        Assert-Test "Confirm safe succeeded" ($csRes.success -eq $true)
    }

    # 14. Trigger SOS
    $sosBody = @{ location = "North Gate"; timestamp = "18:40:00" } | ConvertTo-Json
    $sosRes = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/emergency/sos" -Method Post -Body $sosBody -ContentType "application/json"
    Assert-Test "SOS trigger succeeded" ($sosRes.success -eq $true)

    # 15. Check Active Incidents
    $incs = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/incidents/active" -Method Get
    Assert-Test "Active incidents list returned" ($incs.count -ge 1)
    $incID = $incs.incidents[0].id

    # 16. Acknowledge Incident
    $ackBody = @{ incidentID = $incID; contactID = 101; timestamp = "18:41:00" } | ConvertTo-Json
    $ackRes = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/incidents/acknowledge" -Method Post -Body $ackBody -ContentType "application/json"
    Assert-Test "Incident acknowledgement succeeded" ($ackRes.success -eq $true)

    # 17. Resolve Incident
    $resBody = @{ incidentID = $incID; notes = "Student confirmed safe by responder"; timestamp = "18:45:00" } | ConvertTo-Json
    $resRes = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/incidents/resolve" -Method Post -Body $resBody -ContentType "application/json"
    Assert-Test "Incident resolution succeeded" ($resRes.success -eq $true)

    # 18. Audit Trail
    $audit = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/history/audit" -Method Get
    Assert-Test "Audit trail returned incidents" ($audit.incidents.Count -ge 1)
    Assert-Test "Audit trail returned notifications" ($audit.notifications.Count -ge 1)

    # 19. Events History
    $hist = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/history/events" -Method Get
    Assert-Test "Events history returned sliding window events" ($hist.events.Count -ge 1)

    # 20. End Journey
    $endRes = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/journey/end" -Method Post
    Assert-Test "End journey succeeded" ($endRes.success -eq $true)

} finally {
    Stop-Process -Id $serverProcess.Id -Force -ErrorAction SilentlyContinue
}

Write-Host "=========================================="
if ($allPassed) {
    Write-Host " ALL 20 API INTEGRATION TESTS PASSED! " -ForegroundColor Green
} else {
    Write-Host " SOME API TESTS FAILED! " -ForegroundColor Red
}
Write-Host "=========================================="
