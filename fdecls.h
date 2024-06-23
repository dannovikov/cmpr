rope rope_new(size_t);
int rope_isnull(rope); // 1 if initialized, 0 otherwise
void rope_release(rope*);
span rope_alloc_atleast(rope*,size_t);
void main_loop();
char getch();
void handle_keystroke(char);
void keyboard_help();
void call_llm(span model, json messages, void (*cb)(span));
void read_openai_key();
network_ret call_gpt(json messages, span model); // OpenAI API entry point
network_ret call_gpt_curl(span,span,span); // network helper function
network_ret call_ollama(json messages, span model);
network_ret call_ollama_curl(span,span,span);
void handle_args(int argc, char **argv);
void save_conf();
void check_conf_vars();
void ensure_conf_var(span*, span, span);
void print_config();
void parse_config();
int add_projfile(span); // helper for check_conf_vars
void check_dirs();
void reset_stdin_to_terminal();
void toggle_visual();
void edit_current_block();
void rewrite_current_block_with_llm();
json gpt_message(span role, span message);
void send_to_llm(span prompt);
void handle_openai_response(span, void (*)(span));
void handle_ollama_response(span, void (*)(span));
void replace_code_clipboard();
span block_comment_part(span block);
span block_comment_part_excl(span); // exclusive of comment delimiters
span block_code_part(span);
span block_transforms(span,span fn); // transform block by named function
span comment_to_prompt(span comment);
void ex_expandrefs();
span expand_refs(span);
void expand_refs_rec(span,int);
span chase_ref(span);
span strip_markdown_codeblock(span);
void send_to_clipboard(span prompt);
int file_for_block(span block);
span current_block_language();
span language_for_block(span);
void replace_block_code_part(span new_code);
int launch_editor(char* filename);
void handle_edited_file(char* filename);
span tmp_filename(); // used by 'e' edit command
void new_rev(span, int); // save a new file revision after any change to a block in that file
void update_projfile(int, span, span); // copies a rev into place over an existing file
int copy_file(const char*, const char*);
span pipe_cmd_cmp(span); // should probably be a library method
void compile(); // aka 'B'uild
void start_search();
void perform_search();
void finalize_search();
void search_forward();
void search_backward();
int find_block(span); // find first block containing text
int block_by_id(span); // find a block by id (without hash char)
void start_ex();
void handle_ex_command();
void bootstrap();
void addfile(span);
void addlib(span);
void ex_help();
void set_highlight();
void reset_highlight();
void select_model();
int select_menu(spans opts, int sel); // allows selecting from a short list of options
void print_menu(spans, int);
void page_down();
void page_up();
void print_current_blocks();
void render_block_range(int,int);
void print_physical_lines(span, int);
int print_matching_physical_lines(span, span);
span count_physical_lines(span, int*);
void print_multiple_partial_blocks(int,int);
void print_single_block_with_skipping(int,int);
void cmpr_init(); // handles --init
void print_block(int);
void print_comment(int);
void print_code(int);
int count_blocks();
void clear_display();
void get_code(); // read and index current code
void get_revs(); // read and index revs
spans find_blocks(span); // find the blocks in a file
spans find_blocks_language(span file, span language); // find_blocks helper function dispatching on language
checksum selected_checksum(span); // our selected checksum implementation
void find_all_lines(); // like find_all_blocks, but for lines; applies to the whole project
void index_block_ids();
void ingest(); // updates everything that needs to be updated after code has changed
void ingest();
void index_block_ids();
spans ids_for_block(span);
int block_for_span(span);
span id_for_block(span);
void block_id_jump();
span get_revdir();
span read_file_into(span filename, rope*);
void get_revs_2();
int get_revs_cache_get(span bname, span rev_contents);
checksum scan_checksum(span);
int scan_int(span*);
int parse_int(span); // parse int without mutating
int scan_hex(span*);
int parse_hex(span);
int parse_revfile_cache(span bname, span rev_cache, span rev_contents);
void parse_section_header_line(int *failure, int *section_type, int *block_number, span *rev_cache);
void parse_blocks_lines(int *failure, time_t timestamp, int n_blocks, span rev_contents, span* rev_cache);
void parse_scs_lines(int *failure, int rev_block_idx, span* rev_cache);
void parse_ids_lines(int *failure, int rev_block_idx, span* rev_cache);
void get_revs_cache_put(checksums* working_set, span bname, span content);
void pr_revinfo(span language, spans blocks, int prev_n_revblocks, span contents);
void pr_checksum(checksum);
void pr_relative_span(span,span);
int getkey();
void sbv_display(sbv_state* sbvs);
void sbv_populate(sbv_state* sbvs);
int block_id_match(spans curr_ids, spans rev_ids);
int rev_block_match(sbv_state* sbvs, rev_block* current_revblock);
void select_block_version();
checksums sorted_line_checksums(span);
int cksums_intersection(checksums,checksums);
time_t parse_rev_fname(span);
void main_loop();
void handle_keystroke(char);
void print_menu(spans,int);
int select_menu(spans,int);
void prompt_palette();
spans get_palette();
void apply_prompt(span prompt_name);
void apply_template(span prompt_template);
int block_by_id(span id_no_hash);
void ex_toprefs();
void ex_inrefs();
void ex_expand();
span expand_refs_2(span,span);
void expand_refs_2_rec(span,span,int,int);
span chase_ref_2(span);
span pipe_cmd_cmp(span);
void replace_block(span);
