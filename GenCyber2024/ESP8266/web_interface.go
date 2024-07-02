package main

import (
	"fmt"
	"log"
	"html/template"
	"sync"
	"net/http"
	"time"
)

var client = &http.Client{
	Timeout: 500 * time.Millisecond, // hardcoding timeout as the requests should be instant in our case
}

var (
	laser1 bool
	laser2 bool
	laser3 bool
	led	   bool
	mu     sync.Mutex
)

func main() {
	http.HandleFunc("/", serverTemplate)
	http.HandleFunc("/toggle", toggleHandler)
	http.HandleFunc("/setcolor", colorHandler)

	fmt.Println("[*] Server is running at http://localhost:8080")
	log.Fatal(http.ListenAndServe(":8080", nil))
}

func serverTemplate(w http.ResponseWriter, r *http.Request) {
	indexHtml, err := template.ParseFiles("index.html"); if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	if err := indexHtml.Execute(w, nil); err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}

}

func toggleHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "Invalid Request Method", http.StatusMethodNotAllowed)
		return
	}

	device := r.FormValue("device")
	state := r.FormValue("state")

	mu.Lock()
	defer mu.Unlock()

	switch device {
		case "laser1":
			laser1 = (state == "on")
			laserRequest(1, state)
		case "laser2":
			laser2 = (state == "on")
			laserRequest(2, state)
		case "laser3":
			laser3 = (state == "on")
			laserRequest(3, state)
		case "led":
			led = (state == "on")
		default:
			http.Error(w, "Unknown device", http.StatusBadRequest)
		return
	}

	fmt.Fprintf(w, "Device %s set to %s", device, state)
}

func laserRequest(laserNum int, state string) {
	baseURL := "http://192.168.0.101/laser/"
	action := "off"
	if state == "on" {
		action = "on"
	}

	laserURL := fmt.Sprintf("%s%s?num=%d", baseURL, action, laserNum)

	response, err := http.Get(laserURL); if err != nil {
		log.Printf("[-] Failed to send request to %s: %v\n", laserURL, err)
		return
	}
	defer response.Body.Close()
	if response.StatusCode != http.StatusOK {
		log.Printf("[-] Status Code: %s\n", response.StatusCode)
	}
}

func ledRequest(state string) {
	baseURL := "http://192.168.0.102/led/"
	action := "off"
	if state == "on" {
		action = "on"
	}

	ledURL := fmt.Sprintf("%s%s", baseURL, action)
	response, err := http.Get(ledURL); if err != nil {
		log.Printf("[-] Failed to send request to %s: %v\n", ledURL, err)
		return
	}

	defer response.Body.Close()

	if response.StatusCode != http.StatusOK {
		log.Printf("[-] Status Code: %s\n", response.StatusCode)
	}
}

func colorHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
		return
	}
	color := r.FormValue("color")
	colorRequest(color)
	fmt.Printf("[*] LED color set to %s\n", color)
}

func colorRequest(color string) {
	colorURL := fmt.Sprintf("http://192.168.0.102/led/on?color=%s", color)
	
	response, err := http.Get(colorURL); if err != nil {
		log.Printf("[-] Failed to send request to %s: %v\n", colorURL, err)
		return
	}

	defer response.Body.Close()

	if response.StatusCode != http.StatusOK {
		log.Printf("[-] Status Code: %s\n", response.StatusCode)
	}
}