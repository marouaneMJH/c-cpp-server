// File: load-test.ts

const APP_URL = "http://localhost:8080/";
const CONCURRENCY = 100000; // Number of parallel requests

async function sendRequest(id: number): Promise<number> {
    try {
        const startTime = performance.now();
        const response = await fetch(APP_URL);
        const text = await response.text();
        const endTime = performance.now();
        const duration = endTime - startTime;

        console.log(
            `Client ${id} received ${response.status}: ${text.slice(
                0,
                40
            )}... (${duration.toFixed(2)}ms)`
        );

        return duration;
    } catch (error) {
        console.error(`Client ${id} error:`, (error as Error).message);
        return 0;
    }
}

async function main() {
    const startTotalTime = performance.now();
    const tasks: Promise<number>[] = [];
    const durations: number[] = [];

    // Start all requests immediately
    for (let i = 0; i < CONCURRENCY; i++) {
        const requestPromise = sendRequest(i + 1).then((duration) => {
            if (duration > 0) durations.push(duration);
            return duration;
        });

        tasks.push(requestPromise);
    }

    await Promise.all(tasks);
    const endTotalTime = performance.now();

    // Stats
    const totalTime = endTotalTime - startTotalTime;
    const successfulRequests = durations.length;
    const averageTime =
        successfulRequests > 0
            ? durations.reduce((sum, time) => sum + time, 0) /
              successfulRequests
            : 0;

    console.log(`\n----- Results -----`);
    console.log(`Total time: ${totalTime.toFixed(2)}ms`);
    console.log(`Successful requests: ${successfulRequests}/${CONCURRENCY}`);
    console.log(`Average request time: ${averageTime.toFixed(2)}ms`);
}

main();
