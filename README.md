
# ProjectMemeShop
Computer Software Engineering class project, developed by three students for the Fall 2026 semester.

Initial Project Start Date: 9/16/2026

The Meme Shop is a shopping website to give users the ability to sell and buy memes with memecoins.

Development Tools: 

Front-End: HTML, CSS, & Typescript

Webserver & Back-End: C++ & cpp-httplib. 

Collaboration: Jira, Github, & draw.io.

<img width="631" height="218" alt="architechture_diagram drawio" src="https://github.com/user-attachments/assets/ad0cee1d-98e1-4740-9ae4-2cbaf6f23b45" />

```text
/ProjectMemeShop                            # Root Directory
├── /backend                                # Backend Tier with Cloudflare Worker and database communication configurations
│   ├── worker.js                           # Cloudflare Worker Javascript logic for D1 (SQL) & R2 (No-SQL databases)
│   └── wrangler.jsonc                      # Database communication configurations for Cloudflare's D1 (SQL) & R2 (No-SQL) databases
├── /design                                 # Design Documents
│   ├── architecture-diagram.md             # W.I.P (Do not grade, will include architecture diagram later)
│   ├── frontend-design.md                  # All U.I. and frontend screenshots
│   └── processing-class-diagram.md         # W.I.P (Do not grade, will include class diagram later)
├── /frontend                               # Frontend Tier with all CSS, HTML, and Javascript files for frontend
│   ├── index.html                          # Home page logic W.I.P
│   ├── loginPage.html                      # Login page logic W.I.P
│   └── signUpPage.html                     # Sign Up page logic W.I.P
├── /include                                # C++ Include directory for header files
│   ├── httplib.h                           # CPP-HTTPLIB header file library for web communication in C++
│   └── validation.h                        # W.I.P (Do not grade, will include input validation later)
├── /processing                             # Processing Tier with web server and relevant header implementation files in C++
│   ├── /src
│   │   ├── /main                           # Main directory
│   │   │   ├── webServer.cpp               # W.I.P (Do not grade, not fully commented and partially implemented for SQL)
│   │   ├── /utils                          # Header implementation files
│   │   │   ├── validation.cpp              # W.I.P (Do not grade, will include input validation later)
│   │   │   └── /makefile                   # Build rules and compiler flags for utility header implementations
│   └── /makefile                           # Build rules for svrWeb executable
├── /.gitignore                             # Excludes object, archive, and executable files
├── /makefile                               # Project level build rules and web server hosting (Must have API Key to run)
└── README.md                               # Project overview and set up 
