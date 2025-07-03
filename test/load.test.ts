// File: load-test.ts

const APP_URL = "http://localhost:8080";
const CONCURRENCY = 100000; // Number of clients
const DELAY_MS = 100; // Delay between requests (optional)

async function sendRequest(id: number) {
    try {
        const response = await fetch(APP_URL);
        const text = await response.text();
        console.log(
            `Client ${id} received ${response.status}: ${text.slice(0, 40)}...`
        );
    } catch (error) {
        console.error(`Client ${id} error:`, error.message);
    }
}

async function main() {
    const tasks = [];
    for (let i = 0; i < CONCURRENCY; i++) {
        tasks.push(sendRequest(i + 1));
        await new Promise((r) => setTimeout(r, DELAY_MS)); // Optional pacing
    }

    await Promise.all(tasks);
}

main();
