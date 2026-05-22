package com.edgelab.backend.telemetry;

import jakarta.validation.Valid;
import org.springframework.http.HttpStatus;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api/v1/telemetry")
public class TelemetryController {

    private final TelemetryIngestionService ingestionService;

    public TelemetryController(TelemetryIngestionService ingestionService) {
        this.ingestionService = ingestionService;
    }

    @PostMapping
    @ResponseStatus(HttpStatus.ACCEPTED)
    public TelemetryResponse ingest(@Valid @RequestBody TelemetryRequest request) {
        return ingestionService.ingest(request);
    }
}
